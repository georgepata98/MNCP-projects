{
    TCanvas *c1 = new TCanvas("c1", "Canvas", 800, 600);
    fstream file;
    int nbins=0, i;
    double e0, e1, canal, pulsuri, eroare;
    string a;

    
    cout << "\n   Programul se poate folosi pentru plotarea ca histograma a spectrelor rezultate din MCNP si se pot calcula si ariile picurilor ...\nFisierul cu date din MCNP trebuie sa aiba 3 coloane: Energie, Pulsuri, La suta incertitudine puls." << endl << endl;
    cout << "Nume fisier text cu date (fara .txt): "; cin >> a;

    file.open((a + ".txt").c_str(), ios::in);
    if(!file)
    {
        cout << "\nNu s-a putut accesa fisierul text !" << endl;
        return 1;
    }

    while(1)  // gasire capete histograma (e0 si e1) si numar de bini
    {
        file >> canal >> pulsuri >> eroare;
        if(file.eof()) break;
        if(nbins==0) { e0 = canal; }
        e1 = canal;
        nbins++;
    }
    file.close();

    cout << "\nNr. bini = " << nbins << endl;
    cout << "Capetele histogramei:" << endl;
    cout << "    " << e0 << " [MeV]\n    " << e1 << " [MeV]" << endl;

    TH1F *h1 = new TH1F("h1", "Histograma MCNP", nbins, e0, e1);

    file.open((a + ".txt").c_str(), ios::in);
    while(!file.eof())
    {
        file >> canal >> pulsuri >> eroare;
        i = h1->FindBin(canal + (e1-e0)/nbins/2);  // centrul binului
        h1->SetBinContent(i, pulsuri);
        h1->SetBinError(i, pulsuri*eroare);
    }
    file.close();



    // Eficacitatea absoluta la fotopic
    cout << "\nSe calculeaza eficacitatea absoluta la fotopic? (da/nu): "; cin >> a;
    if(a=="da" || a=="yes" || a=="y")
    {
        double e_st, e_dr, bin_width, bin_err, sum=0;
        int bin_st, bin_dr;  // binul din stanga si dreapta ai picului

        bin_width = (e1-e0)/nbins;
        cout << "Largime bin = " << bin_width << " [MeV]" << endl;
        cout << "Intervalul de energii pe care se integreaza, in [MeV]:" << endl;
        cout << "E_stanga  = "; cin >> e_st;
        cout << "E_dreapta = "; cin >> e_dr;

        for(int k=1; k<=h1->GetNbinsX(); ++k)
        {
            if(e0+k*bin_width < e_st) bin_st = k+1;
            if(e0+k*bin_width == e_st) bin_st = k;
            if(e0+k*bin_width < e_dr) bin_dr = k+1;
            if(e0+k*bin_width == e_dr) bin_dr = k;
        }

        for(int k=1; k<=h1->GetNbinsX(); ++k)
        {
            if(k>=bin_st && k<=bin_dr)
            {
                bin_err = h1->GetBinError(k);
                sum += bin_err*bin_err;
            }
        }
        sum = TMath::Sqrt(sum);

        cout << "\n=> bin_stanga  = " << bin_st << "\n=> bin_dreapta = " << bin_dr;
        cout << "\nEficacitate_detectie(deltaE) = " << h1->Integral(bin_st, bin_dr) << " +/- " << sum << endl;
    }




    // Reprezentare grafica
    h1->SetTitle("MCNP output spectrum");
    h1->GetXaxis()->SetTitle("Energy [MeV]");
    h1->GetYaxis()->SetTitle("Counts");
    h1->Draw("hsit");
    c1->Update();

    return 0;
}
