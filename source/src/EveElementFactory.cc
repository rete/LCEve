
#include <LCEve/EveElementFactory.h>

// -- ROOT headers
#include <ROOT/REveVector.hxx>
#include <ROOT/REveTrack.hxx>
#include <ROOT/REveVSDStructs.hxx>
#include <ROOT/REveEllipsoid.hxx>
#include <TMatrixDEigen.h>
#include <TMatrixDSym.h>
#include <TVectorD.h>

namespace lceve {

  ROOT::REveTrack *EveElementFactory::CreateTrack( const TrackParameters &parameters ) const {
    try {
      ROOT::REveRecTrack trackInfo ;
      trackInfo.fV = parameters.fStartPosition.value() ;
      trackInfo.fP = parameters.fStartMomentum.value() ;
      trackInfo.fSign = parameters.fCharge.value() ;

      auto eveTrack = std::make_unique<ROOT::REveTrack>( &trackInfo, parameters.fPropagator.value() ) ;
      eveTrack->MakeTrack() ;
      if( parameters.fMarkerAttributes ) {
        if( parameters.fMarkerAttributes.value().fColor ) {
          eveTrack->SetMarkerColor( parameters.fMarkerAttributes.value().fColor.value() ) ;
        }
        if( parameters.fMarkerAttributes.value().fSize ) {
          eveTrack->SetMarkerSize( parameters.fMarkerAttributes.value().fSize.value() ) ;
        }
        if( parameters.fMarkerAttributes.value().fStyle ) {
          eveTrack->SetMarkerStyle( parameters.fMarkerAttributes.value().fStyle.value() ) ;
        }
      }
#pragma message "FIXME: Settings twice the color for tracks: 1) main color, 2) line color"
      eveTrack->SetMainColor( parameters.fLineAttributes.value().fColor.value() ) ;
      eveTrack->SetLineColor( parameters.fLineAttributes.value().fColor.value() ) ;
      if( parameters.fLineAttributes.value().fWidth ) {
        eveTrack->SetLineWidth( parameters.fLineAttributes.value().fWidth.value() ) ;
      }
      if( parameters.fMarkerAttributes.value().fStyle ) {
        eveTrack->SetLineStyle( parameters.fLineAttributes.value().fStyle.value() ) ;
      }
      eveTrack->SetPickable( parameters.fPickable.value() ) ;
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
        eveMark.fP = ObjectHelper::ComputeMomentum(
          trackState.fBField.value(), trackState.fOmega.value(),
          trackState.fPhi.value(), trackState.fTanLambda.value() ) ;
        eveTrack->AddPathMark( eveMark );
      }
#pragma message "FIXME: The title is not display as tooltip, the name is usd instead"
      // so here I set the name as the title to display the tooltip correctly.
      // When this is fixed we should switch back to SetName( trkName.str() ) ;
      // eveTrack->SetName( trkName.str() ) ;
      std::stringstream trkName ;
      trkName << "Track p=" << parameters.fStartMomentum.value().Mag() << " GeV";
#pragma message "TODO: Re-write the code for generating the eve track tooltip"
      eveTrack->SetName( trkName.str() ) ;
      eveTrack->SetTitle( trkName.str() ) ;

      return eveTrack.release() ;
    }
    catch( std::bad_optional_access &e ) {
      std::cout << "Couldn't create REveTrack. Missing input parameter(s)" << std::endl ;
    }
    return nullptr ;
  }

  //--------------------------------------------------------------------------

  ROOT::REveEllipsoid *EveElementFactory::CreateVertex( const VertexParameters &parameters ) const {
    auto eveVertex = new ROOT::REveEllipsoid() ;
    std::stringstream vertexName ;
    auto position = parameters.fPosition.value() ;
    vertexName << "Vertex (" << position[0] << ", " << position[1] << ", " << position[2] << ")" ;
    eveVertex->SetName( vertexName.str() ) ;
#pragma message "Implement eve vertex title"
    // set vertex position
    eveVertex->RefMainTrans().SetPos( parameters.fPosition.value().Arr() ) ;
    // Set line attributes
    if( parameters.fLineAttributes ) {
      if( parameters.fLineAttributes.value().fWidth ) {
        eveVertex->SetLineWidth( parameters.fLineAttributes.value().fWidth.value() ) ;
      }    
      if( parameters.fLineAttributes.value().fWidth ) {
        eveVertex->SetLineWidth( parameters.fLineAttributes.value().fWidth.value() ) ;
      }
      if( parameters.fLineAttributes.value().fStyle ) {
        eveVertex->SetLineStyle( parameters.fLineAttributes.value().fStyle.value() ) ;
      }      
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
    return eveVertex ;
  }

  //--------------------------------------------------------------------------

  ROOT::REvePointSet *EveElementFactory::CreateCalorimeterHits( const CaloHitParameters &/*parameters*/ ) const {
#pragma message "TODO: Implement Eve vertex factory method"
    return nullptr ;
  }

  //--------------------------------------------------------------------------

  ROOT::REveElement *EveElementFactory::CreateCluster( const ClusterParameters &/*parameters*/ ) const {
#pragma message "TODO: Implement Eve cluster factory method"
    return nullptr ;
  }

  //--------------------------------------------------------------------------

  ROOT::REveElement *EveElementFactory::CreateRecoParticle( const RecoParticleParameters &/*parameters*/ ) const {
#pragma message "TODO: Implement Eve reco particle factory method"
    return nullptr ;
  }

  //--------------------------------------------------------------------------

  ROOT::REveJetCone *EveElementFactory::CreateJet( const JetParameters &/*parameters*/ ) const {
#pragma message "TODO: Implement Eve jet factory method"
    return nullptr ;
  }

}
