
#include <LCEve/EveElementFactory.h>
#include <LCEve/DrawAttributes.h>

// -- ROOT headers
#include <ROOT/REveVector.hxx>
#include <ROOT/REveTrack.hxx>
#include <ROOT/REveVSDStructs.hxx>
#include <ROOT/REveEllipsoid.hxx>
#include <TMatrixDEigen.h>
#include <TMatrixDSym.h>
#include <TVectorD.h>

// -- std headers
#include <sstream>
#include <iostream>

namespace lceve {

  EveTrack *EveElementFactory::CreateTrack( ROOT::REveTrackPropagator *propagator, const TrackParameters &parameters ) const {
    try {
      ROOT::REveRecTrack trackInfo ;
      trackInfo.fV = parameters.fReferencePoint.value() ;
      trackInfo.fP = parameters.fMomentum.value() ;
      trackInfo.fSign = parameters.fCharge.value() ;
      // Create the track
      auto eveTrack = std::make_unique<EveTrack>( &trackInfo, propagator ) ;
      eveTrack->MakeTrack() ;
      auto defColor = RandomColor( eveTrack.get() ) ;  
      if( parameters.fMarkerAttributes ) {
        auto attr = parameters.fMarkerAttributes.value() ;
        eveTrack->SetMarkerColor( attr.fColor.value_or( RandomColor( eveTrack.get() ) ) ) ;
        if( attr.fSize ) {
          eveTrack->SetMarkerSize( attr.fSize.value() ) ;
        }
        if( attr.fStyle ) {
          eveTrack->SetMarkerStyle( attr.fStyle.value() ) ;
        }
      }
      LineAttributes defAttr ;
      defAttr.fColor = defColor ; defAttr.fWidth = 2 ;
      auto attr = parameters.fLineAttributes.value_or( defAttr ) ;
#pragma message "FIXME: Settings twice the color for tracks: 1) main color, 2) line color"
      eveTrack->SetMainColor( attr.fColor.value_or( defColor ) ) ;
      eveTrack->SetLineColor( attr.fColor.value_or( defColor ) ) ;
      eveTrack->SetLineWidth( attr.fWidth.value_or( 2 ) ) ;
      if( attr.fStyle ) {
        eveTrack->SetLineStyle( attr.fStyle.value() ) ;
      }
      eveTrack->SetPickable( parameters.fPickable.value_or( true ) ) ;
      if( parameters.fUserData ) {
        eveTrack->SetUserData( parameters.fUserData.value() ) ;
      }
      for( auto &trackState : parameters.fTrackStates ) {
        auto type = ROOT::REvePathMark::kReference ;
        if( trackState.fType.value() == TrackStateType::AtEnd ) {
          type = ROOT::REvePathMark::kDecay ;
        }
        ROOT::REvePathMark eveMark( type ) ;
        eveMark.fV = trackState.fReferencePoint.value() ;
        eveMark.fP = trackState.fMomentum.value() ;
        eveTrack->AddPathMark( eveMark );
      }
      // so here I set the name as the title to display the tooltip correctly.
      // When this is fixed we should switch back to SetName( trkName.str() ) ;
      // eveTrack->SetName( trkName.str() ) ;
      
      // Track name
      std::stringstream trkName ;
      trkName << "Track p=" << parameters.fMomentum.value().Mag() << " GeV";
      eveTrack->SetName( trkName.str() ) ;
      
      // Track title
      auto pos = parameters.fReferencePoint.value() ;
      std::stringstream trkTitle ;
      trkTitle << "Track p=" << parameters.fMomentum.value().Mag() << " GeV" << std::endl ;
      trkTitle << "----------------------------------" << std::endl ;
      trkTitle << "Charge = " << parameters.fCharge.value() << std::endl ;
      trkTitle << "Reference point = (" << pos[0] << ", " << pos[1] << ", " << pos[2] << ") cm" << std::endl ;
      trkTitle << "----------------------------------" << std::endl ;
      trkTitle << this->PropertiesAsString( parameters.fProperties ) << std::endl ;
      eveTrack->SetTitle( trkTitle.str() ) ;

      return eveTrack.release() ;
    }
    catch( std::bad_optional_access &e ) {
      std::cout << "Couldn't create REveTrack. Missing input parameter(s)" << std::endl ;
    }
    return nullptr ;
  }

  //--------------------------------------------------------------------------

  EveVertex *EveElementFactory::CreateVertex( const VertexParameters &parameters ) const {
    try {
      auto eveVertex = std::make_unique<EveVertex>() ;
      auto position = parameters.fPosition.value() ;
      // set vertex position
      eveVertex->RefMainTrans().SetPos( position.Arr() ) ;
      auto defColor = RandomColor( eveVertex.get() ) ;
      // Set line attributes
      LineAttributes defAttr ;
      defAttr.fColor = defColor ; defAttr.fWidth = 2 ;
      auto attr = parameters.fLineAttributes.value_or( defAttr ) ;
      eveVertex->SetLineColor( attr.fColor.value_or( defColor ) ) ;
      eveVertex->SetLineWidth( attr.fWidth.value_or( 2 ) ) ;
      if( attr.fStyle ) {
        eveVertex->SetLineStyle( attr.fStyle.value() ) ;
      }      
      // Calculate vertex extent using vertex errors
      // The 6 parameters of a symetric matrix 
      auto errors = parameters.fErrors.value() ;
      TMatrixDSym xxx(3) ;
      xxx(0, 0) = errors[0] ;
      xxx(1, 0) = errors[1] ; xxx(1, 1) = errors[2] ;
      xxx(2, 0) = errors[3] ; xxx(2, 1) = errors[4] ; xxx(2, 2) = errors[5] ;
      // Get eigen vectors
      TMatrixDEigen eig(xxx);
      TVectorD eigenValues = TVectorD( eig.GetEigenValues() ).Sqrt() ;
      TMatrixD eigenVectors = eig.GetEigenVectors();
      ROOT::REveVector baseVectors[3];
      for (int i = 0; i < 3; ++i) {
         baseVectors[i].Set(eigenVectors(0,i), eigenVectors(1,i), eigenVectors(2,i));
#pragma message "FIXME: Find a way to tune the eve vertex extent factor"
         baseVectors[i] *=  eigenValues(i)*500 ;
      }
      eveVertex->SetBaseVectors( baseVectors[0], baseVectors[1], baseVectors[2] ) ;
      eveVertex->Outline();
      
      // Vertex name
      std::stringstream vertexName ;
      vertexName << "Vertex (" << position[0] << ", " << position[1] << ", " << position[2] << ") cm" ;
      eveVertex->SetName( vertexName.str() ) ;
      
      // Vertex title
      std::stringstream vertexTitle ;
      vertexTitle << "Position = (" << position[0] << ", " << position[1] << ", " << position[2] << ") cm" << std::endl ;
      vertexTitle << "Level = " << parameters.fLevel.value() << std::endl ;
      vertexTitle << "Errors: " << FormatReal(errors[0]) << std::endl ;
      vertexTitle << "        " << FormatReal(errors[1]) << " " << FormatReal(errors[2]) << std::endl ;
      vertexTitle << "        " << FormatReal(errors[3]) << " " << FormatReal(errors[4]) << " " << FormatReal(errors[5]) << std::endl ;
      vertexTitle << "----------------------------------" << std::endl ;
      vertexTitle << this->PropertiesAsString( parameters.fProperties ) << std::endl ;
      eveVertex->SetTitle( vertexTitle.str() ) ;
      
      return eveVertex.release() ;
    }
    catch( std::bad_optional_access &e ) {
      std::cout << "Couldn't create EveVertex. Missing input parameter(s)" << std::endl ;
    }
    return nullptr ;
  }

  //--------------------------------------------------------------------------

  EveCluster *EveElementFactory::CreateCluster( const ClusterParameters &parameters ) const {
    try {
      auto eveCluster = std::make_unique<EveCluster>() ;
      auto defColor = RandomColor( eveCluster.get() ) ;
      MarkerAttributes defAttr ;
      defAttr.fColor = defColor ; defAttr.fSize = 3 ; defAttr.fStyle = 4 ;
      auto attr = parameters.fMarkerAttributes.value_or( defAttr ) ;
      eveCluster->SetMarkerColor( attr.fColor.value_or( defColor ) ) ;
      eveCluster->SetMarkerSize( attr.fSize.value_or( 3 ) ) ;
      eveCluster->SetMarkerStyle( attr.fStyle.value_or( 4 ) ) ;
      // fill the cluster with calo hits
      auto caloHits = parameters.fCaloHits.value() ;
      this->PopulateCaloHits( eveCluster.get(), caloHits ) ;
      // generate name and title based on cluster properties
      std::stringstream clusterName ;
      clusterName << "Cluster E=" << parameters.fEnergy.value() << " GeV" ;
      std::stringstream clusterTitle ;
      clusterTitle << "Cluster E=" << parameters.fEnergy.value() << " GeV" << std::endl ;
      if( parameters.fCaloHits ) {
        clusterTitle << "NHits: " << parameters.fCaloHits.value().size() << std::endl ;
      }
      clusterTitle << "----------------------------------" << std::endl ;
      clusterTitle << this->PropertiesAsString( parameters.fProperties ) ;
      eveCluster->SetName( clusterName.str() ) ;
      eveCluster->SetTitle( clusterTitle.str() ) ;
      // release on return
      return eveCluster.release() ;
    }
    catch( std::bad_optional_access &e ) {
      std::cout << "Couldn't create EveCluster. Missing input parameter(s)" << std::endl ;
    }
    return nullptr ;
  }

  //--------------------------------------------------------------------------

  std::unique_ptr<TrackContainer> EveElementFactory::CreateTrackContainer() const {
    return std::make_unique<TrackContainer>() ;
  }

  //--------------------------------------------------------------------------

  std::unique_ptr<VertexContainer> EveElementFactory::CreateVertexContainer() const {
    return std::make_unique<VertexContainer>() ;
  }

  //--------------------------------------------------------------------------

  std::unique_ptr<ClusterContainer> EveElementFactory::CreateClusterContainer() const {
    return std::make_unique<ClusterContainer>() ;
  }

  //--------------------------------------------------------------------------

  std::unique_ptr<RecoParticleContainer> EveElementFactory::CreateRecoParticleContainer() const {
    return std::make_unique<RecoParticleContainer>() ;
  }

  //--------------------------------------------------------------------------

  std::unique_ptr<JetContainer> EveElementFactory::CreateJetContainer() const {
    return std::make_unique<JetContainer>() ;
  }

  //--------------------------------------------------------------------------

  std::unique_ptr<MCParticleContainer> EveElementFactory::CreateMCParticleContainer() const {
    return std::make_unique<MCParticleContainer>() ;
  }

  //--------------------------------------------------------------------------

  std::unique_ptr<CaloHitContainer> EveElementFactory::CreateCaloHitContainer() const {
    return std::make_unique<CaloHitContainer>() ;
  }

  //--------------------------------------------------------------------------

  std::unique_ptr<TrackerHitContainer> EveElementFactory::CreateTrackerHitContainer() const {
    return std::make_unique<TrackerHitContainer>() ;
  }
  
  //--------------------------------------------------------------------------
  
  void EveElementFactory::PopulateCaloHits( CaloHitContainer *container, const std::vector<CaloHitParameters> &caloHits ) const {
    // TODO: re-implement with REveBoxSet when available
    for( auto &c : caloHits ) {
      auto p = c.fPosition.value() ;
      container->SetNextPoint( p[0], p[1], p[2] ) ;
    }
  }
  
  //--------------------------------------------------------------------------

  std::string EveElementFactory::PropertiesAsString( const PropertyMap &properties ) const {
    std::stringstream ss ;
    for( auto &p : properties.items() ) {
      if( p.value().is_object() ) {
        continue;
      }
      ss << p.key() << ": " << p.value().dump() << std::endl ;
    }
    return ss.str() ;
  }

}
