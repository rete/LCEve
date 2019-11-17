
#include <LCEve/LCCollectionBuilder.h>
#include <LCEve/EventDisplay.h>
#include <LCEve/Geometry.h>

#include <TRandom.h>
#include <ROOT/REveScene.hxx>
#include <ROOT/REveJetCone.hxx>

#include <EVENT/LCIO.h>

#include <LCEve/DrawAttributes.h>

namespace lceve {

  LCCollectionConverter::LCCollectionConverter( EventDisplay *lced ) :
    _eventDisplay(lced) {
    /* nop */
  }

  //--------------------------------------------------------------------------

  void LCCollectionConverter::VisualizeEvent( const EVENT::LCEvent *event, REX::REveScene *scene ) {
    auto colnames = event->getCollectionNames() ;
    std::cout << "Loading event in Eve: event " << event->getEventNumber()
              << ", run " << event->getRunNumber()
              << ", " << colnames->size() << " collections" << std::endl ;
    ColorIterator globalIter( ColorIterStrategy::AUTO_ITER ) ;
    auto colsOnly = _eventDisplay->GetSettings().GetReadCollectionNames() ;
    for( auto &colname : *colnames ) {
      auto collection = event->getCollection( colname ) ;

      if( not colsOnly.empty() and std::find(colsOnly.begin(), colsOnly.end(), colname) == colsOnly.end() ) {
        continue ;
      }

      /// Load track collection
      if( collection->getTypeName() == EVENT::LCIO::TRACK ) {
        auto tracks = asVector<EVENT::Track>( collection ) ;
        ColorIterator colors( ColorIterStrategy::AUTO_ITER ) ;
        VisualizeTracks( tracks, colname, scene, colors ) ;
      }
      /// Load calorimeter hits
      else if( collection->getTypeName() == EVENT::LCIO::CALORIMETERHIT ) {
        auto caloHits = asVector<EVENT::CalorimeterHit>( collection ) ;
        ColorIterator colors( ColorIterStrategy::FIXED_COLOR, globalIter.NextColor() ) ;
        VisualizeCaloHits( caloHits, colname, scene, colors ) ;
      }
      /// Load clusters
      else if( collection->getTypeName() == EVENT::LCIO::CLUSTER ) {
        auto clusters = asVector<EVENT::Cluster>( collection ) ;
        ColorIterator colors( ColorIterStrategy::AUTO_ITER, globalIter.NextColor() ) ;
        VisualizeClusters( clusters, colname, scene, colors ) ;
      }
      else if( collection->getTypeName() == EVENT::LCIO::RECONSTRUCTEDPARTICLE ) {
        auto particles = asVector<EVENT::ReconstructedParticle>( collection ) ;
        ColorIterator colors( ColorIterStrategy::AUTO_ITER, globalIter.NextColor() ) ;
        VisualizeRecoParticles( particles, colname, scene, colors ) ;
      }
    }
    // LoadDummyData( scene ) ;
  }

  //--------------------------------------------------------------------------

  void LCCollectionConverter::VisualizeTracks( const std::vector<EVENT::Track*> &tracks,
    const std::string &name,
    REX::REveElement *parent,
    ColorIterator &colorIter) {

    auto barrelData = _eventDisplay->GetGeometry()->GetLayeredCaloData(
      ( dd4hep::DetType::CALORIMETER | dd4hep::DetType::ELECTROMAGNETIC | dd4hep::DetType::BARREL),
      ( dd4hep::DetType::AUXILIARY  |  dd4hep::DetType::FORWARD ) ) ;
    auto endcapData = _eventDisplay->GetGeometry()->GetLayeredCaloData(
      ( dd4hep::DetType::CALORIMETER | dd4hep::DetType::ELECTROMAGNETIC | dd4hep::DetType::ENDCAP),
      ( dd4hep::DetType::AUXILIARY  |  dd4hep::DetType::FORWARD ) ) ;

    auto prop = _eventDisplay->GetGeometry()->CreateTrackPropagator() ;
    prop->SetMaxOrbs(5) ;
    prop->SetMaxR( barrelData->extent[0] ) ;
    prop->SetMaxZ( endcapData->extent[2] ) ;

    REX::REveElement *eveTrackList = new REX::REveElement( name ) ;
    eveTrackList->SetMainColor(kTeal);

    double bfieldV[3] ;
    _eventDisplay->GetGeometry()->GetDetector().field().magneticField( { 0., 0., 0. }  , bfieldV  ) ;
    const double bfield = bfieldV[2]/dd4hep::tesla ;
    unsigned index = 0 ;

    for( auto lcTrack : tracks ) {
      const auto *startTrackState = lcTrack->getTrackState( EVENT::TrackState::AtFirstHit ) ;
      const auto startPoint = startTrackState->getReferencePoint() ;
      const double omega = startTrackState->getOmega() ;
      const double pt(bfield* 2.99792e-4 / std::fabs(omega));
      const double px(pt * std::cos(startTrackState->getPhi()));
      const double py(pt * std::sin(startTrackState->getPhi()));
      const double pz(pt * startTrackState->getTanLambda());

      auto trackInfo = new REX::REveRecTrack() ;
      trackInfo->fV.Set( startPoint[0]*0.1, startPoint[1]*0.1, startPoint[2]*0.1 ) ;
      trackInfo->fP.Set( px, py, pz ) ;
      trackInfo->fSign = 0 ;
      if (std::numeric_limits<float>::epsilon() < std::fabs(omega)) {
        trackInfo->fSign = static_cast<int>(omega / std::fabs(omega));
      }

      auto eveTrack = new REX::REveTrack( trackInfo, prop ) ;
      eveTrack->MakeTrack() ;
      eveTrack->SetName( Form("Track %d", index) ) ;
      eveTrack->SetStdTitle() ;
      eveTrack->SetMainColor( colorIter.NextColor() ) ;
      eveTrack->SetLineWidth(2);
      eveTrack->SetPickable(true);

      eveTrackList->AddElement( eveTrack ) ;
    }
    parent->AddElement( eveTrackList ) ;
    index++ ;
  }

  //--------------------------------------------------------------------------

  void LCCollectionConverter::VisualizeCaloHits( const std::vector<EVENT::CalorimeterHit*> &caloHits,
    const std::string &name,
    REX::REveElement *parent,
    ColorIterator &colorIter ) {

    auto eveCaloHitList = new REX::REvePointSet( name ) ;
    eveCaloHitList->SetMarkerColor( colorIter.NextColor() ) ;
    eveCaloHitList->SetMarkerSize( 3 ) ;
    eveCaloHitList->SetMarkerStyle( 4 ) ;

    for( auto caloHit : caloHits ) {
      auto p = caloHit->getPosition() ;
      eveCaloHitList->SetNextPoint( p[0]*0.1, p[1]*0.1, p[2]*0.1 ) ;
    }
    parent->AddElement( eveCaloHitList ) ;
  }


  //--------------------------------------------------------------------------

  void LCCollectionConverter::VisualizeClusters( const std::vector<EVENT::Cluster*> &clusters,
    const std::string &name,
    REX::REveElement *parent,
    ColorIterator &colorIter ) {

    auto eveClusterList = new REX::REveElement( name ) ;
    unsigned index = 0 ;
    for( auto cluster : clusters ) {
      auto eveCluster = new REX::REveElement( Form( "Cluster %d", index ) ) ;
      eveClusterList->AddElement( eveCluster ) ;
      VisualizeCaloHits( cluster->getCalorimeterHits(), "CaloHits", eveCluster, colorIter ) ;
      index++ ;
    }
    parent->AddElement( eveClusterList ) ;
  }

  void LCCollectionConverter::VisualizeRecoParticles( const std::vector<EVENT::ReconstructedParticle*> &particles,
    const std::string &name,
    REX::REveElement *parent,
    ColorIterator &colorIter ) {

    auto barrelData = _eventDisplay->GetGeometry()->GetLayeredCaloData(
      ( dd4hep::DetType::CALORIMETER | dd4hep::DetType::HADRONIC | dd4hep::DetType::BARREL),
      ( dd4hep::DetType::AUXILIARY  |  dd4hep::DetType::FORWARD ) ) ;
    auto endcapData = _eventDisplay->GetGeometry()->GetLayeredCaloData(
      ( dd4hep::DetType::CALORIMETER | dd4hep::DetType::HADRONIC | dd4hep::DetType::ENDCAP),
      ( dd4hep::DetType::AUXILIARY  |  dd4hep::DetType::FORWARD ) ) ;

    auto eveParticleList = new REX::REveElement( name ) ;
    unsigned index = 0 ;
    for( auto particle : particles ) {
      auto associatedParticles = particle->getParticles() ;
      auto color = colorIter.NextColor() ;
      if( not associatedParticles.empty() ) {
        auto jet = new REX::REveJetCone( Form( "Particle %d (jet)", index) ) ;
        jet->SetCylinder( barrelData->extent[1], endcapData->extent[3] ) ;
        // TODO calculate jet cone

        jet->SetFillColor(color);
        jet->SetLineColor(TColor::GetColorBright(color));
        eveParticleList->AddElement( jet ) ;
      }
      else {
        auto eveParticle = new REX::REveElement( Form( "Particle %d", index) ) ;
        ColorIterator colIter( ColorIterStrategy::FIXED_COLOR, color ) ;
        if( not particle->getTracks().empty() ) {
          VisualizeTracks( particle->getTracks(), "Tracks", eveParticle, colIter ) ;
        }
        if( not particle->getClusters().empty() ) {
          VisualizeClusters( particle->getClusters(), "Clusters", eveParticle, colIter ) ;
        }
        // TODO add vertex collection drawing
        eveParticleList->AddElement( eveParticle ) ;
      }
      index++ ;
    }
    parent->AddElement( eveParticleList ) ;
  }

  //--------------------------------------------------------------------------

  // void LCCollectionConverter::LoadDummyData( REX::REveScene *scene ) {
  //
  //   TRandom &r = *gRandom;
  //   auto prop = new REX::REveTrackPropagator();
  //   prop->SetMagFieldObj(new REX::REveMagFieldDuo(350, -3.5, 2.0));
  //   prop->SetMaxR(300);
  //   prop->SetMaxZ(600);
  //   prop->SetMaxOrbs(6);
  //
  //   auto trackHolder = new REX::REveElement("Tracks");
  //
  //   double v = 0.2;
  //   double m = 5;
  //
  //   int N_Tracks = 10 + r.Integer(20);
  //   for (int i = 0; i < N_Tracks; i++)
  //   {
  //     TParticle* p = new TParticle();
  //
  //     int pdg = 11* (r.Integer(2) -1);
  //     p->SetPdgCode(pdg);
  //
  //     p->SetProductionVertex(r.Uniform(-v,v), r.Uniform(-v,v), r.Uniform(-v,v), 1);
  //     p->SetMomentum(r.Uniform(-m,m), r.Uniform(-m,m), r.Uniform(-m,m)*r.Uniform(1, 3), 1);
  //     auto track = new REX::REveTrack(p, 1, prop);
  //     track->MakeTrack();
  //     track->SetMainColor(kBlue);
  //     track->SetName(Form("RandomTrack_%d", i));
  //     trackHolder->AddElement(track);
  //  }
  //  scene->AddElement(trackHolder);
  // }

}
