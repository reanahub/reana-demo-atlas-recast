#ifndef MyEventSelection_MyEventSelectionAlg_H
#define MyEventSelection_MyEventSelectionAlg_H

#include <EventLoop/Algorithm.h>
#include <TH1F.h>
#include <map>

class MyEventSelectionAlg : public EL::Algorithm
{
  // put your configuration variables here as public variables.
  // that way they can be set directly from CINT and python.
 public:
  // float cutValue;
  
  std::string m_xsecfile;

  float m_lumi_in_ifb;

 private:
  TH1F* m_hist_nmuons; //!
  TH1F* m_hist_regions; //!
  std::map<int,float> m_xsecmap; //!
  float m_sumofweights = 0; //!

  
  // variables that don't get filled at submission time should be
  // protected from being send from the submission node to the worker
  // node (done by the //!)
 public:
  // this is a standard constructor
  MyEventSelectionAlg ();
  
  // these are the functions inherited from Algorithm
  virtual EL::StatusCode setupJob (EL::Job& job);
  virtual EL::StatusCode fileExecute ();
  virtual EL::StatusCode histInitialize ();
  virtual EL::StatusCode changeInput (bool firstFile);
  virtual EL::StatusCode initialize ();
  virtual EL::StatusCode execute ();
  virtual EL::StatusCode postExecute ();
  virtual EL::StatusCode finalize ();
  virtual EL::StatusCode histFinalize ();
  
  // this is needed to distribute the algorithm to the workers
  ClassDef(MyEventSelectionAlg, 1);
};

#endif
