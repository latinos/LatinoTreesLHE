


TH1F * DrawOverflow(TH1F *h) {
  // This function paint the histogram h with an extra bin for overflows
  UInt_t nx = h->GetNbinsX()+1;
  Double_t *xbins= new Double_t[nx+1];
  for (UInt_t i=0;i<nx;i++)
    xbins[i]=h->GetBinLowEdge(i+1);
  xbins[nx]=xbins[nx-1]+h->GetBinWidth(nx);
  char *tempName= new char[strlen(h->GetName())+10];
  sprintf(tempName,"%swtOverFlow",h->GetName());
  // Book a temporary histogram having ab extra bin for overflows
  TH1F *htmp = new TH1F(tempName, h->GetTitle(), nx, xbins);
  //---- style
  htmp->SetLineColor(h->GetLineColor());
  htmp->SetLineWidth(h->GetLineWidth());
  htmp->SetLineStyle(h->GetLineStyle());
  // Reset the axis labels
  htmp->SetXTitle(h->GetXaxis()->GetTitle());
  htmp->SetYTitle(h->GetYaxis()->GetTitle());
  // Fill the new hitogram including the extra bin for overflows
  for (UInt_t i=1; i<=nx; i++)
    htmp->Fill(htmp->GetBinCenter(i), h->GetBinContent(i));
  // Fill the underflows
  htmp->Fill(h->GetBinLowEdge(1)-1, h->GetBinContent(0));
  // Restore the number of entries
  htmp->SetEntries(h->GetEntries());
  // FillStyle and color
  htmp->SetFillStyle(h->GetFillStyle());
  htmp->SetFillColor(h->GetFillColor());
  return htmp;
}

#include "TDRStyle.cc"

void drawKl(std::string nameInFileRootOne, 
            std::string nameInFileRootTwo,
            float kl = 1,
            std::string var = "etaSC", int NBIN = 1000, float MIN = 0, float MAX = 1000, std::string varHR = ""
          ) {
  
   
  TDRStyle();
  gStyle->SetTitleYOffset(1.1);
  gStyle->cd();
  
  if (varHR == "") {
    varHR = var;
  }
  gStyle->SetOptStat(0);
  
  
  TFile* f_1;
  TFile* f_2;
  
  f_1 = new TFile (nameInFileRootOne.c_str());
  f_2 = new TFile (nameInFileRootTwo.c_str());

  TTree* t_1;
  TTree* t_2;
  
  t_1 = (TTree*) f_1 -> Get ("tree");
  t_2 = (TTree*) f_2 -> Get ("tree");
          
  
  TH1F* h_1;
  TH1F* h_2;
  
  h_1 = new TH1F("h_1","before",NBIN,MIN,MAX);
  h_2 = new TH1F("h_2","after",NBIN,MIN,MAX);
     
  h_1->Sumw2();
  h_2->Sumw2();
  
  TString weight = Form ("evtWeight");
//   TString weight = Form ("evtWeight * mqq>20");
  //   TString weight = Form ("1");
  TString toDraw;
    
  toDraw = Form ("%s >> h_1",var.c_str());
  t_1->Draw(toDraw.Data(),weight.Data(),"goff");

  toDraw = Form ("%s >> h_2",var.c_str());
  t_2->Draw(toDraw.Data(),weight.Data(),"goff");
  
  //
  //---- the h_2 as drawn from ttree is the 
  //---- EWK correction to the Born production
  //
  
  TH1F* h_3 = (TH1F*) h_2 -> Clone ("afterMinusKl");
  
  h_2->Scale(1-kl);
  h_3->Scale(1+kl);
  
  h_2->Add(h_1);
  h_3->Add(h_1);
  
  //---- 
  //----    h_1 + (1 +/- kl) * h_2
  //---- 
  
  
  //----
  
  
  h_1->SetMarkerSize  (0);
  h_1->SetMarkerColor (kBlue);
  h_1->SetLineColor   (kBlue);
  
  h_2->SetMarkerSize  (0);
  h_2->SetMarkerColor (kRed);
  h_2->SetLineColor   (kRed);
  
  h_3->SetMarkerSize  (0);
  h_3->SetMarkerColor (kOrange+2);
  h_3->SetLineColor   (kOrange+2);
  
  
  h_1->SetLineWidth(2);
  h_2->SetLineWidth(2);
  h_3->SetLineWidth(2);
  
  h_1->SetLineStyle(2);
  h_2->SetLineStyle(3);
  h_3->SetLineStyle(4);
  
  h_1->GetXaxis()->SetTitle(varHR.c_str());
  h_2->GetXaxis()->SetTitle(varHR.c_str());
  h_3->GetXaxis()->SetTitle(varHR.c_str());
  
  
  TLegend* leg = new TLegend(0.80,0.70,0.95,0.90);
  leg->AddEntry(h_1, h_1->GetTitle(), "l");
  leg->AddEntry(h_2, Form("k_{#lambda} =  %1.2f", kl) , "l");
  leg->AddEntry(h_3, Form("k_{#lambda} =  %1.2f", -kl), "l");
  leg->SetFillStyle(0);

  //---- overflow bin
  h_1 = DrawOverflow(h_1);
  h_2 = DrawOverflow(h_2);
  h_3 = DrawOverflow(h_3);
  
  //---- draw
  TCanvas* cn = new TCanvas ("cn","cn",900,600);
  
  h_1->Draw("hist");
  h_2->Draw("hist same");
  h_3->Draw("hist same");
  
  leg->Draw();
  cn->SetGrid();

  //---- draw
  TCanvas* cnNorm = new TCanvas ("cnNorm","cnNorm",900,600);
  
  h_1->DrawNormalized("hist");
  h_2->DrawNormalized("hist same");
  h_3->DrawNormalized("hist same");
  
  leg->Draw(); 
  cnNorm->SetGrid(); 
  
  //---- draw ratio
  TCanvas* cnRatio = new TCanvas ("cnRatio","cnRatio",900,600);
  
  // Define the ratio plot
  TH1F *h_ratio_2  = (TH1F*) h_2->Clone("h_ratio_2");
  h_ratio_2->GetYaxis()->SetTitle("after / before");
  h_ratio_2->SetLineColor(kRed);
  h_ratio_2->SetStats(0);     
  h_ratio_2->Divide(h_1);
  h_ratio_2->SetMarkerStyle(21);
  h_ratio_2->Draw("histo");      
  h_ratio_2->GetYaxis()->SetRangeUser(0.7, 1.3);
  
  TH1F *h_ratio_3  = (TH1F*) h_3->Clone("h_ratio_3");
  h_ratio_3->GetYaxis()->SetTitle("after / before");
  h_ratio_3->SetLineColor(kOrange+2);
  h_ratio_3->SetStats(0);     
  h_ratio_3->Divide(h_1);
  h_ratio_3->SetMarkerStyle(21);
  h_ratio_3->Draw("histo same");      
  
  leg->Draw();
  cnRatio->SetGrid(); 
  
  
}


