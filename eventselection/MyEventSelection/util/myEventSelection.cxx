#include "xAODRootAccess/Init.h"
#include "SampleHandler/SampleHandler.h"
#include "SampleHandler/ScanDir.h"
#include "SampleHandler/ToolsDiscovery.h"
#include "EventLoop/Job.h"
#include "EventLoop/DirectDriver.h"
#include "SampleHandler/DiskListLocal.h"
#include <TSystem.h>

#include <iostream>

#include "MyEventSelection/MyEventSelectionAlg.h"

int main( int argc, char* argv[] ) {

  // Take the submit directory from the input if provided:
  if( argc <= 1 ) return 1;

  std::string submitDir = argv[1];
  std::string fileList  = argv[2];

  // Set up the job for xAOD access:
  xAOD::Init().ignore();

  // Construct the samples to run on:
  SH::SampleHandler sh;

  SH::readFileList (sh, "sample", fileList);

  // Set the name of the input TTree. It's always "CollectionTree"
  // for xAOD files.
  sh.setMetaString( "nc_tree", "CollectionTree" );

  // Print what we found:
  sh.print();

  // Create an EventLoop job:
  EL::Job job;
  job.sampleHandler( sh );
  job.options()->setDouble (EL::Job::optMaxEvents, -1);

  // Add our analysis to the job:
  MyEventSelectionAlg* alg = new MyEventSelectionAlg();
  alg->m_xsecfile = argv[3];
  alg->m_lumi_in_ifb = atof(argv[4]);
  
  job.algsAdd( alg );

  // Run the job using the local/direct driver:
  EL::DirectDriver driver;
  driver.submit( job, submitDir );
  
  return 0;
}
