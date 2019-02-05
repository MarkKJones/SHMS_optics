void plotResults(){

  
  int dirs[5] = {1813,1814,1815,1817,6620};
  gROOT->SetBatch(true);


  //generate the output pdf here
  TCanvas *canvas = new TCanvas("canvas","plots", 700, 700);
  canvas->SetFillColor(0);
  canvas->SetBorderMode(0);
  canvas->SetBorderSize(0);
  canvas->SetFrameFillColor(0);
  canvas->SetFrameBorderMode(0);
  canvas->SetLeftMargin(0.15);
  std::string pdf_file_name = "optimization_output.pdf";

  string ff[3] = {"runsv1_31Jan19.root","opt_v1.root","opt_v2.root"};
  TFile *f[3];
  TGraph *gg[3][5][9];
  TH1D *h_z[3][5];
  TH1D *h_ytar[3][5];
  TH2D *h2_xy[3][5][3];
  
  canvas->Update();
  for (int ifile=0; ifile<3; ifile++){
    f[ifile] = new TFile(ff[ifile].c_str());

    //cout<<"opening file: "<<ff[ifile].c_str()<<endl;
    for (int ik=0; ik<5; ik++){
      f[ifile]->cd(Form("run_%d",dirs[ik]));

      h_z[ifile][ik] = (TH1D*)gDirectory->Get("zVer");
      h_ytar[ifile][ik] = (TH1D*)gDirectory->Get("yTar");
      if (ifile==0){h_z[ifile][ik]->SetLineColor(kRed); h_ytar[ifile][ik]->SetLineColor(kRed);}
      else if(ifile==1){h_z[ifile][ik]->SetLineColor(kBlue); h_ytar[ifile][ik]->SetLineColor(kBlue);}
      else{h_z[ifile][ik]->SetLineColor(kGreen); h_ytar[ifile][ik]->SetLineColor(kGreen);}

      for (int ll=0;ll<3;ll++){
	h2_xy[ifile][ik][ll] = (TH2D*)gDirectory->Get(Form("xySieve_%d",ll));
      } 

      //cout<<"opening run directory: run_"<<dirs[ik]<<endl;
      for (int ii=0; ii<9; ii++){
	gg[ifile][ik][ii] = (TGraph*)gDirectory->Get(Form("Graph;%d",ii));
	//cout<<"getting the plot: "<<ii<<endl;
	
       	if (gg[ifile][ik][ii]){
	  gg[ifile][ik][ii]->SetName(Form("gg_%d_%d_%d",ifile,ik,ii));

	  if (ifile==0){
	    gg[ifile][ik][ii]->SetMarkerStyle(20);
	    gg[ifile][ik][ii]->SetMarkerColor(kRed);
	  }
	  else if (ifile==1){
	    gg[ifile][ik][ii]->SetMarkerStyle(21);
	    gg[ifile][ik][ii]->SetMarkerColor(kBlue);
	  }
	  else{
	    gg[ifile][ik][ii]->SetMarkerStyle(22);
	    gg[ifile][ik][ii]->SetMarkerColor(kGreen);
	  }
	}
	
      }//end loop ii through graphs
    }//end loop ik through run numbers
  }//end loop through files

  
  for (int irun=0; irun<5; irun++){
    for (int nfile=0; nfile<3; nfile++){
      if (nfile==0){
	h_z[nfile][irun]->Draw();
      }
      else{
	h_z[nfile][irun]->Draw("same");
      }
    }
    canvas->Print( (pdf_file_name + "(").c_str());

    for (int nfile=0; nfile<3; nfile++){
      if (nfile==0){
	h_ytar[nfile][irun]->Draw();
      }
      else{
	h_ytar[nfile][irun]->Draw("same");
      }
    }
    canvas->Print( (pdf_file_name + "(").c_str());


    //  canvas->Divide(2,2);canvas->Update();
      for (int ifoil=0; ifoil<3; ifoil++){
	for (int nfile=0; nfile<3; nfile++){
	//	canvas->cd(ifoil);
	if (h2_xy[nfile][irun][ifoil]){
	  if (nfile==0){h2_xy[nfile][irun][ifoil]->SetTitle(Form("Original (red), run %d, foil %d", dirs[irun],ifoil));}
	  else if (nfile==1){h2_xy[nfile][irun][ifoil]->SetTitle(Form("Excludes 1919 (blue), run %d, foil %d", dirs[irun],ifoil));}
	  else{h2_xy[nfile][irun][ifoil]->SetTitle(Form("Excludes 30deg runs (green), run %d, foil %d", dirs[irun],ifoil));}
	  
	  h2_xy[nfile][irun][ifoil]->Draw("colz");
	  canvas->Print( (pdf_file_name + "(").c_str());
	}
      }
    }






    
    for (int iplot=0; iplot<9; iplot++){
      for (int ifile=0; ifile<3; ifile++){
	if (gg[ifile][irun][iplot]){
	  if (ifile==0){
	    gg[ifile][irun][iplot]->Draw("ap");
	    gg[ifile][irun][iplot]->GetYaxis()->SetTitleOffset(1.7);
	    int nfoil=0;
	    if (iplot>=4 && iplot<=6){nfoil=1;}
	    else if(iplot>6){nfoil=2;}
	    gg[ifile][irun][iplot]->SetTitle(Form("Run_%d, Foil%d", dirs[irun],nfoil));
	    gg[ifile][irun][iplot]->Draw("ap");
	  }
	 else {
	   gg[ifile][irun][iplot]->Draw("psame");	 
	  }
	}
      }
      canvas->Print( (pdf_file_name + "(").c_str());
    }
  }


  
  
    canvas->Print( (pdf_file_name + ")").c_str());
  
}//end plotResults
