#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <MyEventSelection/MyEventSelectionAlg.h>


// Infrastructure include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"

#include <TTree.h>

// ASG status code check
#include <AsgTools/MessageCheck.h>


#include "xAODMuon/MuonContainer.h"
#include "xAODEventInfo/EventInfo.h"

#include <iostream>

#include "xAODCutFlow/CutBookkeeper.h"
#include "xAODCutFlow/CutBookkeeperContainer.h"


// this is needed to distribute the algorithm to the workers
ClassImp(MyEventSelectionAlg)



MyEventSelectionAlg :: MyEventSelectionAlg ()
{
  // Here you put any code for the base initialization of variables,
  // e.g. initialize all pointers to 0.  Note that you should only put
  // the most basic initialization here, since this method will be
  // called on both the submission and the worker node.  Most of your
  // initialization code will go into histInitialize() and
  // initialize().
}



EL::StatusCode MyEventSelectionAlg :: setupJob (EL::Job& job)
{
  // Here you put code that sets up the job on the submission object
  // so that it is ready to work with your algorithm, e.g. you can
  // request the D3PDReader service or add output files.  Any code you
  // put here could instead also go into the submission script.  The
  // sole advantage of putting it here is that it gets automatically
  // activated/deactivated when you add/remove the algorithm from your
  // job, which may or may not be of value to you.


  job.useXAOD ();
  ANA_CHECK(xAOD::Init());

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyEventSelectionAlg :: histInitialize ()
{
  // Here you do everything that needs to be done at the very
  // beginning on each worker node, e.g. create histograms and output
  // trees.  This method gets called before any input files are
  // connected.

  
  Info("histInitialize()", "xsecfile %s", m_xsecfile.c_str());

  TTree t;
  t.ReadFile(m_xsecfile.c_str()); 
  int channel=0; float crossSection=0; 
  t.SetBranchAddress("id",&channel);
  t.SetBranchAddress("xsec",&crossSection);
  for(int i=0;i<t.GetEntries();i++) {
    t.GetEntry(i);
    Info("histInitialize()","xsec for %i is %f",channel,crossSection);
    m_xsecmap[channel] = crossSection;
  }

  m_hist_nmuons  = new TH1F ("nMuons", "number of Muons", 16, -0.5, 15.5);
  wk()->addOutput(m_hist_nmuons);

  m_hist_regions = new TH1F ("regions", "the event regions", 2, -0.5, 1.5);
  wk()->addOutput(m_hist_regions);



  
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyEventSelectionAlg :: fileExecute ()
{
  // Here you do everything that needs to be done exactly once for every
  // single file, e.g. collect a list of all lumi-blocks processed


  xAOD::TEvent* event = wk()->xaodEvent(); // you should have already added this as described before

  // Now, let's find the actual information
  const xAOD::CutBookkeeperContainer* completeCBC = 0;
  if(!event->retrieveMetaInput(completeCBC, "CutBookkeepers").isSuccess()){
    Error("initializeEvent()","Failed to retrieve CutBookkeepers from MetaData! Exiting.");
    return EL::StatusCode::FAILURE;
  }


  TString xStream=""; 
  int maxcycle=-1;
  const xAOD::CutBookkeeper* allEventsCBK = 0;

  for ( auto cbk : *completeCBC ) {
    std::cout << cbk->nameIdentifier() << " : " << cbk->name()
	      << " : desc = " << cbk->description()
	      << " : inputStream = " << cbk->inputStream()
	      << " : outputStreams = " << (cbk->outputStreams().size() ? cbk->outputStreams()[0] : "")
	      << " : cycle = " << cbk->cycle()
	      << " : allEvents = " << cbk->nAcceptedEvents()
	      << " : sow = " << cbk->sumOfEventWeights()
	      << std::endl;
    
    if ( cbk->name() == "AllExecutedEvents" && TString(cbk->inputStream()).Contains("StreamDAOD")){ //guess DxAOD flavour
      xStream = TString(cbk->inputStream()).ReplaceAll("Stream","");
      std::cout << "xStream = " << xStream << "  (i.e. indentified DxAOD flavour)" << std::endl;
    }
    if ( cbk->name() == "AllExecutedEvents" && cbk->inputStream() == "StreamAOD" && cbk->cycle() > maxcycle){
      maxcycle = cbk->cycle();
      allEventsCBK = cbk;
    }
  }
  
  m_sumofweights = allEventsCBK->sumOfEventWeights();

  std::cout << "sum of weights: " << m_sumofweights << std::endl;
  
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyEventSelectionAlg :: changeInput (bool firstFile)
{
  // Here you do everything you need to do when we change input files,
  // e.g. resetting branch addresses on trees.  If you are using
  // D3PDReader or a similar service this method is not needed.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyEventSelectionAlg :: initialize ()
{
  ANA_CHECK_SET_TYPE (EL::StatusCode);
  
  // Here you do everything that you need to do after the first input
  // file has been connected and before the first event is processed,
  // e.g. create additional histograms based on which variables are
  // available in the input files.  You can also create all of your
  // histograms and trees in here, but be aware that this method
  // doesn't get called if no events are processed.  So any objects
  // you create here won't be available in the output if you have no
  // input events.

  xAOD::TEvent* event = wk()->xaodEvent(); // you should have already added this as described before

  // as a check, let's see the number of events in our xAOD
  Info("initialize()", "Number of events = %lli", event->getEntries() ); // print long long int

  
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyEventSelectionAlg :: execute ()
{
  ANA_CHECK_SET_TYPE (EL::StatusCode);
  
  // Here you do everything that needs to be done on every single
  // events, e.g. read in  ANA_CHECK_SET_TYPE (EL::StatusCode);

  // This method is the mirror image of initialize(), meaning it gets
  // called after the last event has been processed on the worker node
  // and allows you to finish up any objects you created in
  // initialize() before they are written to disk.  This is actually
  // fairly rare, since this happens separately for each worker node.
  // Most of the time you want to do your post-processing on the
  // submission node after all your histogram outputs have been
  // merged.  This is different from histFinalize() in that it only
  // gets called on worker nodes that processed input events.

  xAOD::TEvent* event = wk()->xaodEvent();


  const xAOD::EventInfo* eventInfo = 0; 
  ANA_CHECK( event->retrieve( eventInfo, "EventInfo" ) ); 

  Info("execute()", "MC channel Number %i", eventInfo->mcChannelNumber());
  
  const xAOD::MuonContainer* muons = 0;
  ANA_CHECK(event->retrieve( muons, "Muons" ));
  
  Info("execute()", "number of muons %i", muons->size());
  
  Info("execute()", "Lumi %f [ifb], Xsec: %f [pb], Ngen: %f ", m_lumi_in_ifb, m_xsecmap[eventInfo->mcChannelNumber()], m_sumofweights);
  float event_weight = m_lumi_in_ifb * 1000 * m_xsecmap[eventInfo->mcChannelNumber()] / m_sumofweights;
  
  Info("execute()", "weight: %f", event_weight);
    
  m_hist_nmuons->Fill(muons->size(), event_weight);

  if(muons->size() < 3){
    m_hist_regions->Fill(0.0, event_weight);
  }
  else{
    m_hist_regions->Fill(1.0, event_weight);
  }

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode MyEventSelectionAlg :: postExecute ()
{
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode MyEventSelectionAlg :: histFinalize ()
{
  // This method is the mirror image of histInitialize(), meaning it
  // gets called after the last event has been processed on the worker
  // node and allows you to finish up any objects you created in
  // histInitialize() before they are written to disk.  This is
  // actually fairly rare, since this happens separately for each
  // worker node.  Most of the time you want to do your
  // post-processing on the submission node after all your histogram
  // outputs have been merged.  This is different from finalize() in
  // that it gets called on all worker nodes regardless of whether
  // they processed input events.
  return EL::StatusCode::SUCCESS;
}


EL::StatusCode MyEventSelectionAlg :: finalize ()
{
  return EL::StatusCode::SUCCESS;
}
