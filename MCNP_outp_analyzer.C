{
    TCanvas *c1 = new TCanvas("c1", "Canvas", 800, 600);
    fstream file;
    int nbins, i;
    double e0, e1, canal, pulsuri, eroare;
    string a;

    
    cout << "\n   Programul se poate folosi pentru plotarea ca histograma a spectrelor rezultate din MCNP si se pot calcula si ariile picurilor ...\nFisierul cu date din MCNP trebuie sa aiba 3 coloane: Energie, Pulsuri, La suta incertitudine puls." << endl << endl;
    cout << "Nr. total de bini ai histogramei (= nr. linii fisier):\n     "; cin >> nbins;
    cout << "Capetele din stanga si dreapta ale histogramei, in [MeV]" << endl;
    cout << "     "; cin >> e0;
    cout << "     "; cin >> e1;
    TH1F *h1 = new TH1F("h1", "Histograma MCNP", nbins, e0, e1);
    cout << "Nume fisier text cu date (fara .txt): "; cin >> a;


    file.open((a + ".txt").c_str(), ios::in);
    if(!file)
    {
        cout << "\nNu s-a putut accesa fisierul text !" << endl;
        return 1;
    }
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
        cout << "Intervalul de energii pe care se integreaza, in [MeV]:" << endl;
        cout << "E_stanga  = "; cin >> e_st;
        cout << "E_dreapta = "; cin >> e_dr;

        if(e_st == (e_st/bin_width)*bin_width)
        {
            bin_st = e_st/bin_width;
        }
        else { bin_st = e_st/bin_width+1; }

        if(e_dr == (e_dr/bin_width)*bin_width)
        {
            bin_dr = e_dr/bin_width;
        }
        else { bin_dr = e_dr/bin_width+1; }

        for(int k=1; k<=h1->GetNbinsX(); ++k)
        {
            if(k>=bin_st && k<=bin_dr)
            {
                bin_err = h1->GetBinError(k);
                sum += bin_err*bin_err;
            }
        }
        sum = TMath::Sqrt(sum);

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
