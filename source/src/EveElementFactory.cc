
#include <LCEve/EveElementFactory.h>

#include <ROOT/REveVector.hxx>
#include <ROOT/REveTrack.hxx>
#include <ROOT/REveVSDStructs.hxx>

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
      // FIXME: Should we set this twice here ?
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
      // FIXME: The title is not display as tooltip,
      // so here I set the name as the title to display the tooltip correctly.
      // When this is fixed we should switch back to SetName( trkName.str() ) ;
      // eveTrack->SetName( trkName.str() ) ;
      std::stringstream trkName ;
      trkName << "Track p=" << parameters.fStartMomentum.value().Mag() << " GeV";
      // TODO re-write the code for the track title.
      // For the time being, just use the name
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

  }

  //--------------------------------------------------------------------------

  ROOT::REvePointSet *EveElementFactory::CreateCalorimeterHits( const CaloHitParameters &parameters ) const {

  }

  //--------------------------------------------------------------------------

  ROOT::REveElement *EveElementFactory::CreateCluster( const ClusterParameters &parameters ) const {

  }

  //--------------------------------------------------------------------------

  ROOT::REveElement *EveElementFactory::CreateRecoParticle( const RecoParticleParameters &parameters ) const {

  }

  //--------------------------------------------------------------------------

  ROOT::REveJetCone *EveElementFactory::CreateJet( const JetParameters &parameters ) const {

  }

}
