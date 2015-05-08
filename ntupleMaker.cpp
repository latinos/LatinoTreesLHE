// to compile:
// c++ -o ntupleMaker.exe `root-config --glibs --cflags` -lm ntupleMaker.cpp
//
// to use:
// ./ntupleMaker.exe fileLHE outputFileRoot
//

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <functional>
#include <assert.h>


#include "LHEF.h"



#include "TTree.h"
#include "TFile.h"
#include "TLorentzVector.h"
#include "Math/Vector3D.h"
#include "Math/Vector4D.h"

#include "TRandom.h"

struct ptsort: public std::binary_function<TLorentzVector, TLorentzVector, bool> {
 bool operator () (const TLorentzVector & x, const TLorentzVector & y) {
  return (x.Perp () < y.Perp () ) ;
 }
} ;

TLorentzVector buildP (const LHEF::HEPEUP & event, int iPart) {
 TLorentzVector dummy ;
 dummy.SetPxPyPzE (
  event.PUP.at (iPart).at (0), // px
                   event.PUP.at (iPart).at (1), // py
                   event.PUP.at (iPart).at (2), // pz
                   event.PUP.at (iPart).at (3) // E
 ) ;
 return dummy ;
}




class myParticle: public TLorentzVector {
 
public:
 
 myParticle(): TLorentzVector() {};
 myParticle( const myParticle& other ) : TLorentzVector( other ) {
  pdgId_ = other.GetPdgId();
 };
 
 void SetPdgId(int pdgId) {pdgId_ = pdgId;}
 int GetPdgId() const {return pdgId_;}
 
private :
 int pdgId_;
};


class myTree {
 
public:
 
 myTree();
 ~myTree() {};
 
 TTree* tree;
 float pt1_;
 float eta1_;
 float phi1_;
 
 float pt2_;
 float eta2_;
 float phi2_;
 
 float mt_;
 
 void fillTree (std::string fileNameLHE);
 void Write(TFile& out);
 void Init();
 
};


myTree::myTree(){
 tree = new TTree("tree","tree");
 
 tree->Branch("pt1",&pt1_,"pt1/F");
 tree->Branch("eta1",&eta1_,"eta1/F");
 tree->Branch("phi1",&phi1_,"phi1/F");
 tree->Branch("pt2",&pt2_,"pt2/F");
 tree->Branch("eta2",&eta2_,"eta2/F");
 tree->Branch("phi2",&phi2_,"phi2/F");
 tree->Branch("mt",&mt_,"mt/F");
 
}

void myTree::Init(){
 
 pt1_  = -99;
 eta1_ = -99;
 phi1_ = -99;
 mt_   = -99;
 
}


void myTree::fillTree(std::string fileNameLHE){
 std::ifstream ifs (fileNameLHE.c_str ()) ;
 LHEF::Reader reader (ifs) ;
 
 long ieve = 0 ;
 long selected = 0 ;
 
 std::cout << " reading " << fileNameLHE << std::endl;
 
 // loop over events
 while ( reader.readEvent () ) {
  ieve++;
  if (ieve % 10000 == 0) std::cout << "event " << ieve << "\n" ;
  
  //---- initialize variables in tree
  Init();
  
  std::vector<TLorentzVector> v_tlv_all_leptons ; //leptons
  
  // loop over particles in the event
  // and fill the variables of leptons and quarks
  for (int iPart = 0 ; iPart < reader.hepeup.IDUP.size (); ++iPart) {
   
   // outgoing particles
   if (reader.hepeup.ISTUP.at (iPart) == 1) {
    
    //leptons
    if (abs (reader.hepeup.IDUP.at (iPart)) == 11 || abs(reader.hepeup.IDUP.at (iPart)) == 13 ||   abs(reader.hepeup.IDUP.at (iPart)) == 15) {
     
     
     TLorentzVector dummy1 (
      reader.hepeup.PUP.at (iPart).at (0), // px
                            reader.hepeup.PUP.at (iPart).at (1), // py
                            reader.hepeup.PUP.at (iPart).at (2), // pz
                            reader.hepeup.PUP.at (iPart).at (3) // E
     ) ;
     
     v_tlv_all_leptons.push_back (dummy1) ;
    }
   }
  } // outgoing particles
  
  // sorting in pt
  sort (v_tlv_all_leptons.rbegin (), v_tlv_all_leptons.rend (), ptsort ()) ;
  
  //---- fill the ntuple
  if (v_tlv_all_leptons.size()>0) {
   pt1_  = v_tlv_all_leptons.at (0).Pt ();
   eta1_ = v_tlv_all_leptons.at (0).Eta ();
   phi1_ = v_tlv_all_leptons.at (0).Phi ();
  }
  if (v_tlv_all_leptons.size()>1) {
   pt2_  = v_tlv_all_leptons.at (1).Pt ();
   eta2_ = v_tlv_all_leptons.at (1).Eta ();
   phi2_ = v_tlv_all_leptons.at (1).Phi ();
  }
  
  
  tree->Fill();
 }
}


void myTree::Write(TFile& out) {
 out.cd();
 tree->Write();
}



///---- LHE dumper ----

int main (int argc, char **argv) {
 // Open a stream connected to an event file:
 if (argc < 3) exit (1) ;
 
 std::cout << " Input LHE = " << argv[1] << std::endl;
 std::cout << " Output ROOT = " << argv[2] << std::endl;
 
 myTree myT;
 myT.fillTree (std::string(argv[1])) ;
 
 TFile output (argv[2], "recreate") ;
 output.cd() ;
 myT.Write(output);
 output.Close();
 
}



