


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

void drawKlComplete(std::string nameInFileRootOne, 
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
  
  //
  //   h_2 = NLO - LO
  //   h_1 = LO
  //
  //   h_2 / h_1 - 1.536*10^-3 = C1
  //
  //   h_NLO      = h_1 + h_2
  //   h_NLO (kl) = h_1 + h_2 + C_1 * (kl-1) * h_1 + (kl^2-1) * C_2 * h_1
  //       C_2 =  - 1.536*10^-3 / (1+kl * 1.536*10^-3)
  //   h_NLO (kl) = h_1 + h_2 + (h_2 / h_1 - 1.536*10^-3) * (kl-1) * h_1  - (kl^2-1) * 1.536*10^-3 / (1+kl * 1.536*10^-3) * h_1
  //              = h_1 + h_2 + h_2 * (kl-1) - 1.536*10^-3 * (kl-1) * h_1 - (kl^2-1) * 1.536*10^-3 / (1+kl * 1.536*10^-3) * h_1 
  //              = h_1 + h_2 * kl           - ( 1.536*10^-3 * (kl-1)  + (kl^2-1) * 1.536*10^-3 / (1+kl * 1.536*10^-3) )  * h_1 
  //   
  //   h_C1 = h_2 / h_1 - 1.536*10^-3 
  //   
  
  TH1F* h_NLO = (TH1F*) h_1 -> Clone ("NLO");
  TH1F* h_NLO_BSM_plus  = (TH1F*) h_1 -> Clone ("NLOplus");
  TH1F* h_NLO_BSM_minus = (TH1F*) h_1 -> Clone ("NLOminus");
  
  h_NLO -> Add (h_2, 1);
  
//   h_NLO_BSM_plus -> Scale ( 1. - ( 1.536*10^-3 * (kl-1)  + (kl^2-1) * 1.536*10^-3 / (1+kl * 1.536*10^-3) )  );
  h_NLO_BSM_plus -> Scale ( 1. - ( 1.536/1000 * (kl-1)  + (kl*kl-1) * 1.536/1000. / (1+kl * 1.536/1000) )  );
  h_NLO_BSM_plus -> Add (h_2, kl );
  
  h_NLO_BSM_minus -> Scale ( 1. - ( 1.536/1000 * (-kl-1)  + (kl*kl-1) * 1.536/1000. / (1-kl * 1.536/1000) )  );
  h_NLO_BSM_minus -> Add (h_2, -kl );

  TH1F* h_C1 = (TH1F*) h_2 -> Clone ("C1");
  h_C1->Divide (h_1);
  for (int ibin=0; ibin<(h_C1->GetNbinsX()+1); ibin++) {
    h_C1->SetBinContent (ibin+1, h_C1->GetBinContent (ibin+1) - 1.536/1000 );
  }
  
  //----
  
  
  h_C1->SetMarkerSize  (1);
  h_C1->SetMarkerColor (kBlue+2);
  h_C1->SetLineColor   (kBlue+2);
  
  
  h_1->SetMarkerSize  (0);
  h_1->SetMarkerColor (kBlue);
  h_1->SetLineColor   (kBlue);
  
  h_NLO->SetMarkerSize  (0);
  h_NLO->SetMarkerColor (kRed);
  h_NLO->SetLineColor   (kRed);
  
  h_NLO_BSM_plus->SetMarkerSize  (0);
  h_NLO_BSM_plus->SetMarkerColor (kOrange+2);
  h_NLO_BSM_plus->SetLineColor   (kOrange+2);
  
  h_NLO_BSM_minus->SetMarkerSize  (0);
  h_NLO_BSM_minus->SetMarkerColor (kOrange-2);
  h_NLO_BSM_minus->SetLineColor   (kOrange-2);
  
  
  h_1            ->SetLineWidth(2);
  h_NLO          ->SetLineWidth(2);
  h_NLO_BSM_plus ->SetLineWidth(2);
  h_NLO_BSM_minus->SetLineWidth(2);
  
  h_1            ->SetLineStyle(2);
  h_NLO          ->SetLineStyle(3);
  h_NLO_BSM_plus ->SetLineStyle(4);
  h_NLO_BSM_minus->SetLineStyle(5);
  
  h_1->GetXaxis()->SetTitle(varHR.c_str());
  h_NLO->GetXaxis()->SetTitle(varHR.c_str());
  h_NLO_BSM_plus->GetXaxis()->SetTitle(varHR.c_str());
  h_NLO_BSM_minus->GetXaxis()->SetTitle(varHR.c_str());
  h_C1->GetXaxis()->SetTitle(varHR.c_str());
  
  
  TLegend* leg = new TLegend(0.80,0.70,0.95,0.90);
  leg->AddEntry(h_1, "LO", "l");
  leg->AddEntry(h_NLO, "NLO", "l");
  leg->AddEntry(h_NLO_BSM_plus, Form("k_{#lambda} =  %1.2f", kl) , "l");
  leg->AddEntry(h_NLO_BSM_minus, Form("k_{#lambda} =  %1.2f", -kl), "l");
  leg->SetFillStyle(0);

  //---- overflow bin
  h_1   = DrawOverflow(h_1);
  h_NLO = DrawOverflow(h_NLO);
  h_NLO_BSM_plus  = DrawOverflow(h_NLO_BSM_plus);
  h_NLO_BSM_minus = DrawOverflow(h_NLO_BSM_minus);
  h_C1 = DrawOverflow(h_C1);
   
  //---- draw
  TCanvas* cn = new TCanvas ("cn","cn",900,600);
  
  h_1->Draw("hist");
  h_NLO->Draw("hist same");
  h_NLO_BSM_plus ->Draw("hist same");
  h_NLO_BSM_minus->Draw("hist same");
  
  leg->Draw();
  cn->SetGrid();

  //---- draw
  TCanvas* cnNorm = new TCanvas ("cnNorm","cnNorm",900,600);
  
  h_1->DrawNormalized("hist");
  h_NLO->DrawNormalized("hist same");
  h_NLO_BSM_plus->DrawNormalized("hist same");
  h_NLO_BSM_minus->DrawNormalized("hist same");
  
  leg->Draw(); 
  cnNorm->SetGrid(); 
  
  //---- draw ratio
  TCanvas* cnRatio = new TCanvas ("cnRatio","cnRatio",900,600);
  
  // Define the ratio plot
  TH1F *h_ratio_2  = (TH1F*) h_NLO_BSM_plus->Clone("h_ratio_2");
  h_ratio_2->GetYaxis()->SetTitle("XX / LO");
  h_ratio_2->SetLineColor(kRed);
  h_ratio_2->SetStats(0);     
  h_ratio_2->Divide(h_1);
  h_ratio_2->SetMarkerStyle(21);
  h_ratio_2->Draw("histo");      
  h_ratio_2->GetYaxis()->SetRangeUser(0.7, 1.3);
  
  TGraph* gr_ratio = new TGraph();
  for (int ibin=0; ibin<h_ratio_2->GetNbinsX(); ibin++) {
    gr_ratio->SetPoint (ibin, h_ratio_2->GetBinCenter(ibin+1), h_ratio_2->GetBinContent(ibin+1));
  }
  gr_ratio->SetName("gr_ratio");
  gr_ratio->Draw("PL");
  gr_ratio->SaveAs("ratio.root");
  
  
  TH1F *h_ratio_3  = (TH1F*) h_NLO_BSM_minus->Clone("h_ratio_3");
  h_ratio_3->GetYaxis()->SetTitle("XX / LO");
  h_ratio_3->SetLineColor(kOrange+2);
  h_ratio_3->SetStats(0);     
  h_ratio_3->Divide(h_1);
  h_ratio_3->SetMarkerStyle(21);
  h_ratio_3->Draw("histo same");      


  TH1F *h_ratio_4  = (TH1F*) h_NLO->Clone("h_ratio_4");
  h_ratio_4->GetYaxis()->SetTitle("XX / LO");
  h_ratio_4->SetLineColor(kRed);
  h_ratio_4->SetStats(0);     
  h_ratio_4->Divide(h_1);
  h_ratio_4->SetMarkerStyle(21);
  h_ratio_4->Draw("histo same");      
  
  leg->Draw();
  cnRatio->SetGrid(); 
  
  
  
  
  
  
  
  
  //---- draw C1
  TCanvas* cnC1 = new TCanvas ("cnC1","cnC1",900,600);
  
  h_C1->Draw ();  
  h_C1->GetYaxis()->SetTitle("C1");
  h_C1->SetStats(0);     
  h_C1->SetMarkerStyle(21);
  h_C1->Draw("histo");      
  
  h_C1->SaveAs("C1.root");
  
  
  
}


