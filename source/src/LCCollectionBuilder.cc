
#include <LCEve/LCCollectionBuilder.h>
#include <LCEve/EventDisplay.h>
#include <LCEve/Geometry.h>

#include <TRandom.h>
#include <ROOT/REveScene.hxx>
#include <ROOT/REveJetCone.hxx>

#include <EVENT/LCIO.h>

#include <LCEve/DrawAttributes.h>

namespace lceve {

  template <typename T>
  inline std::string FormatReal( const T &val ) {
    std::stringstream sstr ;
    sstr << std::scientific << (val >= 0. ? "+" : "") << val ;
    return sstr.str() ;
  }


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

  void LCCollectionConverter::VisualizeTracks( const std::vector<EVENT::Track*> &itracks,
    const std::string &name,
    REX::REveElement *parent,
    ColorIterator &colorIter) {

    // get a copy and sort by momentum
    auto tracks = itracks ;
    std::sort( tracks.begin(), tracks.end(), [this](auto lhs, auto rhs){
      auto lhsMomentum = this->ComputeMomentum( lhs->getTrackState( EVENT::TrackState::AtFirstHit ) ) ;
      auto rhsMomentum = this->ComputeMomentum( rhs->getTrackState( EVENT::TrackState::AtFirstHit ) ) ;
      return ( rhsMomentum.Mag() < lhsMomentum.Mag() ) ;
    } ) ;

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
    unsigned index = 0 ;

    for( auto lcTrack : tracks ) {

      const auto *startTrackState = lcTrack->getTrackState( EVENT::TrackState::AtFirstHit ) ;
      const auto startPoint = startTrackState->getReferencePoint() ;
      auto momentumAtStart = ComputeMomentum( startTrackState ) ;

      REX::REveRecTrack trackInfo;
      trackInfo.fV.Set( startPoint[0]*0.1, startPoint[1]*0.1, startPoint[2]*0.1 ) ;
      trackInfo.fP = momentumAtStart ;
      trackInfo.fSign = 0 ;
      if (std::numeric_limits<float>::epsilon() < std::fabs(startTrackState->getOmega())) {
        trackInfo.fSign = static_cast<int>(startTrackState->getOmega() / std::fabs(startTrackState->getOmega()));
      }

      REX::REvePathMark endPositionMark(REX::REvePathMark::kReference);
      const auto *endTrackState = lcTrack->getTrackState( EVENT::TrackState::AtLastHit ) ;
      const auto positionAtEnd = endTrackState->getReferencePoint() ;
      const auto momentumAtEnd = ComputeMomentum( endTrackState ) ;
      endPositionMark.fV.Set(positionAtEnd[0]*0.1, positionAtEnd[1]*0.1, positionAtEnd[2]*0.1);
      endPositionMark.fP = momentumAtEnd ;

      REX::REvePathMark caloPositionMark(REX::REvePathMark::kDecay);
      const auto *caloTrackState = lcTrack->getTrackState( EVENT::TrackState::AtCalorimeter ) ;
      const auto positionAtCalo = caloTrackState->getReferencePoint() ;
      caloPositionMark.fV.Set(positionAtCalo[0]*0.1, positionAtCalo[1]*0.1, positionAtCalo[2]*0.1);

      std::stringstream trkName ;
      trkName << "Track p=" << momentumAtStart.Mag() << " GeV";
      std::stringstream trkTitle ;
      trkTitle << std::string(30, '-') << " Track " << std::string(30, '-') << "\n" ;
      trkTitle << "Momentum                                            " << FormatReal(momentumAtStart.Mag()) << " GeV\n"
               << "Charge                                              " << trackInfo.fSign << "\n"
               << "Type                                                " << lcTrack->getType() << "\n"
               << "dE/dX                                               " << FormatReal(lcTrack->getdEdx()) << "\n"
               << "dE/dX error                                         " << FormatReal(lcTrack->getdEdxError()) << "\n"
               << "Chi2                                                " << FormatReal(lcTrack->getChi2()) << "\n"
               << "Ndf                                                 " << lcTrack->getNdf() << "\n" ;
      trkTitle << GetTrackStateDescription( lcTrack->getTrackState( EVENT::TrackState::AtOther) ) ;
      trkTitle << GetTrackStateDescription( lcTrack->getTrackState( EVENT::TrackState::AtIP) ) ;
      trkTitle << GetTrackStateDescription( lcTrack->getTrackState( EVENT::TrackState::AtFirstHit) ) ;
      trkTitle << GetTrackStateDescription( lcTrack->getTrackState( EVENT::TrackState::AtLastHit) ) ;
      trkTitle << GetTrackStateDescription( lcTrack->getTrackState( EVENT::TrackState::AtCalorimeter) ) ;
      trkTitle << GetTrackStateDescription( lcTrack->getTrackState( EVENT::TrackState::AtVertex) ) ;

      auto eveTrack = new REX::REveTrack( &trackInfo, prop ) ;
      eveTrack->MakeTrack() ;
      eveTrack->AddPathMark(endPositionMark);
      eveTrack->AddPathMark(caloPositionMark);
      // FIXME: The title is not display as tooltip,
      // so here I set the name as the title to display the tooltip correctly.
      // When this is fixed we should switch back to SetName( trkName.str() ) ;
      // eveTrack->SetName( trkName.str() ) ;
      eveTrack->SetName( trkTitle.str() ) ;
      eveTrack->SetTitle( trkTitle.str() ) ;
      eveTrack->SetMainColor( colorIter.NextColor() ) ;
      eveTrack->SetLineWidth(2);
      eveTrack->SetPickable(true);

      eveTrackList->AddElement( eveTrack ) ;
      index++ ;
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

  void LCCollectionConverter::VisualizeClusters( const std::vector<EVENT::Cluster*> &iclusters,
    const std::string &name,
    REX::REveElement *parent,
    ColorIterator &colorIter ) {

    auto clusters = iclusters ;
    std::sort( clusters.begin(), clusters.end(), [](auto lhs, auto rhs){
      return ( lhs->getEnergy() < rhs->getEnergy() ) ;
    } ) ;
    auto eveClusterList = new REX::REveElement( name ) ;
    unsigned index = 0 ;
    for( auto cluster : clusters ) {
      auto eveCluster = new REX::REvePointSet() ;
      eveCluster->SetMarkerColor( colorIter.NextColor() ) ;
      eveCluster->SetMarkerSize( 3 ) ;
      eveCluster->SetMarkerStyle( 4 ) ;
      auto &caloHits = cluster->getCalorimeterHits() ;
      for( auto caloHit : caloHits ) {
        auto p = caloHit->getPosition() ;
        eveCluster->SetNextPoint( p[0]*0.1, p[1]*0.1, p[2]*0.1 ) ;
      }
      eveClusterList->AddElement( eveCluster ) ;

      std::stringstream clusterName ;
      clusterName << "Cluster E=" << FormatReal(cluster->getEnergy()) << " GeV" ;

      std::stringstream clusterTitle ;
      clusterTitle << std::string(29, '-') << " Cluster " << std::string(29, '-') << "\n" ;
      clusterTitle << "Energy                                               " << FormatReal(cluster->getEnergy()) << " GeV\n"
                   << "Energy error                                         " << FormatReal(cluster->getEnergyError()) << " GeV\n"
                   << "Position               " << FormatReal(cluster->getPosition()[0]) << " ,"
                                                << FormatReal(cluster->getPosition()[1]) << " ,"
                                                << FormatReal(cluster->getPosition()[2]) << "\n"
                   << "Position error         " << FormatReal(cluster->getPositionError()[0]) << " ,"
                                                << FormatReal(cluster->getPositionError()[1]) << " ,"
                                                << FormatReal(cluster->getPositionError()[2]) << "\n"
                   << "NHits                                                " << cluster->getCalorimeterHits().size() << "\n"
                   << "IPhi                                                 " << FormatReal(cluster->getIPhi()) << "\n"
                   << "ITheta                                               " << FormatReal(cluster->getITheta()) << "\n" ;
      auto &pids = cluster->getParticleIDs() ;
      clusterTitle << GetParticleIDsDescription( pids ) ;
      // eveCluster->SetName( clusterName.str() ) ;
      eveCluster->SetName( clusterTitle.str() ) ;
      eveCluster->SetTitle( clusterTitle.str() ) ;
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
          VisualizeTracks( particle->getTracks(), Form("Tracks (%d)",(int)particle->getTracks().size()), eveParticle, colIter ) ;
        }
        if( not particle->getClusters().empty() ) {
          VisualizeClusters( particle->getClusters(), Form("Clusters (%d)", (int)particle->getClusters().size()), eveParticle, colIter ) ;
        }
        std::stringstream particleTitle ;
        particleTitle << GetParticleIDsDescription( particle->getParticleIDs() ) ;
        eveParticle->SetName( particleTitle.str() ) ;
        eveParticle->SetTitle( particleTitle.str() ) ;
        // TODO add vertex collection drawing
        eveParticleList->AddElement( eveParticle ) ;
      }
      index++ ;
    }
    parent->AddElement( eveParticleList ) ;
  }

  //--------------------------------------------------------------------------

  std::string LCCollectionConverter::GetTrackStateDescription( const EVENT::TrackState *const trkState ) const {
    if( nullptr == trkState ) {
      return "" ;
    }
    std::stringstream description ;
    auto locationStr = GetTrackStateLocationAsString(trkState) ;
    description << "------------------------ Track state: " << locationStr << " " << std::string(28 - locationStr.size(), '-') << "\n"
                << "D0                                                  " << FormatReal(trkState->getD0()) << "\n"
                << "Phi                                                 " << FormatReal(trkState->getPhi()) << "\n"
                << "Omega                                               " << FormatReal(trkState->getOmega()) << "\n"
                << "Z0                                                  " << FormatReal(trkState->getZ0()) << "\n"
                << "TanLambda                                           " << FormatReal(trkState->getTanLambda()) << "\n"
                << "Reference point       " << FormatReal(trkState->getReferencePoint()[0]) << ", "
                                            << FormatReal(trkState->getReferencePoint()[1]) << ", "
                                            << FormatReal(trkState->getReferencePoint()[2]) << "\n";
    return description.str() ;
  }

  //--------------------------------------------------------------------------

  std::string LCCollectionConverter::GetTrackStateLocationAsString( const EVENT::TrackState *const trkState ) const {
    const int location = trkState->getLocation() ;
    switch( location ) {
      case EVENT::TrackState::AtOther:
        return "AtOther" ;
      case EVENT::TrackState::AtIP:
        return "AtIP" ;
      case EVENT::TrackState::AtFirstHit:
        return "AtFirstHit" ;
      case EVENT::TrackState::AtLastHit:
        return "AtLastHit" ;
      case EVENT::TrackState::AtCalorimeter:
        return "AtCalorimeter" ;
      case EVENT::TrackState::AtVertex:
        return "AtVertex" ;
      default:
        return "Unknown" ;
    }
  }

  //--------------------------------------------------------------------------

  std::string LCCollectionConverter::GetParticleIDsDescription( const std::vector<EVENT::ParticleID*> &pids ) const {
    if( pids.empty() ) {
      return "" ;
    }
    std::stringstream sstr ;
    sstr << "------------------------ Particle IDs ------------------------\n" ;
    for( auto pid : pids ) {
      sstr << "----- Algorithm                " << pid->getAlgorithmType() << "\n"
           << "Type                                                " << pid->getType() << "\n"
           << "PDG                                                 " << pid->getPDG() << "\n"
           << "Likelihood                                          " << pid->getLikelihood() << "\n" ;
      auto &params = pid->getParameters() ;
      if( not params.empty() ) {
        sstr << "Parameters";
                // "                      "
        for( unsigned int i=0 ; i<params.size() ; i++ ) {
          if( i%3 ) {
            sstr << "\n                      ";
          }
          sstr << params[i] << "  ";
        }
        sstr << "\n";
      }
    }
    return sstr.str() ;
  }

}
