//
//  Tintegral.cpp
//
//
//  Created by Ivan on 9/12/21.
//  Comments and minor corrections made by Marie and Matthew
//
#ifndef __CINT__

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

#include <TROOT.h>
#include <TSystem.h>
#include <TCanvas.h>
#include <TImage.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TF2.h>
#include <TMath.h>
#include <TStyle.h>
#include <TRandom.h>
#include "TMinuit.h"
#include "TMultiGraph.h"


class Qint {
public:
  // canvas constants
  // this is nc_j1ded for the creation of the graph
  static const float left_margin; //Graph Info (Left Side)
  static const float right_margin; //Graph Info (Right Side)
  static const float top_margin; //Graph Info (Top)
  static const float bot_margin; //Graph Info (Bottom)

  // constants for integrals
  // these are nc_j1ded for the computation of integrals
  static const double pii; //Pi Constant
  static const double pi2; //Pi Divided by 2
  static const double pi4; //Pi Divided by 4
  static const double s0; //Not used

  //Coordinates of x-ray
  double X0;
  double Y0;
  double Z0;
  // rectangular pixel borders
  // x = [b_i, b_i1] and y = [c_j, c_j1]
  double b_i, b_i1, c_j, c_j1;
  static const double PIX_WDTH; //Width of the pixel

    int Nint;  // number of integrations
    static const int Npix=5; // Number of pixels
    static const int N_X=11; // Number of x values // can make lower to reduce computation time?
    static const int N_Y=11; // Number of y values
    static const int N_Z=11; // Number of z values
    double qij[N_X][N_Y][N_Z][Npix][Npix]; //array for data
    /*
    11 x 11 x 11 x 5 x 5: 5-dimensional array
    33275 rows with 6 entries
    */
    static const double Valid;
    double alpha_n; //alpha_n variable (from paper)
    double Qtot; //Total Charge
    vector <double> vsum, s_idx; //Not used?
    vector <double> vx, vy; //Not used

    static const int NgX; //X axis for histogram (?)
    static const int NgY;  //Y axis for histogram (?)

    TH2D * StermN; //Pointer for histogram (?)
    TH2D * hProf; //Pointer for histogram (?)


public:
    //Functions
    Qint( double x0, double y0, double z0);  //: X0(x0), Y0(y0), Z0(z0){}
    double Tint(int n);
    int Tintegral ( void );
    int Sinx ( double zz );
    double Qj(void);  //double x0, double y0, double z0);
    int PlotQj(void);
    int PlotProf(void);
    ~Qint(){ };
};

// initialization of static members
const float Qint::left_margin =  (float)0.06;
const float Qint::right_margin = (float)0.006;
const float Qint::top_margin =   (float)0.01;
const float Qint::bot_margin =   (float)0.04;

const double Qint::pii(TMath::Pi());
const double Qint::pi2(pii/2.);  //=TMath::Pi()/2.;
const double Qint::pi4(pii/4.);  //=TMath::Pi()/4.;
const double Qint::s0=0.01;
const double Qint::PIX_WDTH=2.0; //Width of the pixel
const double Qint::Valid=0.0005;

const int Qint::NgX=60;
const int Qint::NgY=20;

// constructor
Qint::Qint( double x0, double y0, double z0):
X0(x0), //X Coordinate
Y0(y0), //Y Coordinate
Z0(z0)  //Z Coordinate
{
    Nint=200.;
    b_i1= 1.0;
    b_i=-1.0;
    c_j1= 1.0;
    c_j=-1.0;
    //Titling Graph
    char title[40];
    sprintf(title, "#Integrals");
    // make histogram with `NgX` bins ranging from 0 to 3 on X axis
    // and `NgY` bins from 0 to 1 on Y axis
    StermN = new TH2D(title,title, NgX, 0., 3.0,
                                   NgY, 0., 1.0);
    StermN->SetStats(0); //Removing Statistics Box from plot

    //Another histogram graph
    sprintf(title, "Profile");
    hProf = new TH2D(title,title, 5, 0., 5., 5, 0., 5.);
    hProf->SetStats(0);

    // coordinates of the pixel that is in the center of the grid
    int Xc = Npix/2; //Equals 2, which is the index for the central pixel row
    int Yc = Npix/2; //Equals 2, which is the index for the central pixel column

    // cvs file for the lookup table
    std::ofstream qint_output("qint_output.csv");
    qint_output << "X X-Ray Coordinate" << "," << "Y X-Ray Coordinate"
      << "," << "Z X-Ray Coordinate" << "," << "X Pixel Index"
      << "," <<  "Y Pixel Index" << "," << "Charge Fraction" << "\n";
    cout << "Beginning Calculation " << endl;
    for (int xp=0; xp<Npix; xp++) {  //Iterating over the x pixels
        b_i=(xp-Npix/2.)*PIX_WDTH; //Defining Left border of pixel
        b_i1=b_i+PIX_WDTH; //Defining Right border of pixel
        for (int yp=0; yp<Npix; yp++) { //Iterating over the y pixels
            c_j=(yp-Npix/2.)*PIX_WDTH; //Defining Upper border of pixel
            c_j1=c_j+PIX_WDTH; //Defining Lower border of pixel
            for (int x_ray=0; x_ray<N_X; x_ray++) { //Iterating over number of x positions for x-ray
                X0=x_ray*(PIX_WDTH/2.)/(N_X-1); //Calculating x position of x-ray
                for (int y_ray=0; y_ray<N_Y; y_ray++) { //Iterating over number of y positions for x-ray
                    Y0=y_ray*(PIX_WDTH/2.)/(N_Y-1);  //Calculating y position of x-ray
                    

                    Z0 = 0; //Z0 = 0 is a special case for the z-axis (Avoids cumbersome if statement)
                    qij[x_ray][y_ray][0][xp][yp] = 0.0;
                    //The equation goes to zero because
                    //z_ray, which is z0 in the paper, is an argument in a sine (see Eq. 11)
                    //Physical Interpretation of this is that when the height is zero,
                    //all the charge should go to one pixel
                    if (xp == Xc && yp == Yc) {
                        qij[x_ray][y_ray][0][xp][yp] = 1.0; //Pixel takes all charge
                    }
                    double q = qij[x_ray][y_ray][0][xp][yp]; //Writing for the z = 0 case
                    qint_output << x_ray << "," << y_ray
                        << "," << 0 << "," << xp
                        << "," << yp << "," << q << "\n";

                    
                    for (int z_ray=1; z_ray<N_Z; z_ray++) { //Iterating over number of z positions for x-ray 
                        //except 0 as that was delt with earlier
                        Z0=z_ray/(N_Z-1); //Calculating z position of x-ray
                        //Note: These coordiantes are divided by the height (h in the paper)
                        //of the free field region, in the paper it is 8um
                        //This is why Z0 is not scaled by anything and X0 and Y0 are scaled by PIX_WIDTH/2.
                        //Divided by 2. due to the symmetry in the center
                        qij[x_ray][y_ray][z_ray][xp][yp]=Qj();
                        
                        double q = qij[x_ray][y_ray][z_ray][xp][yp];
                        qint_output << x_ray << "," << y_ray
                          << "," << z_ray << "," << xp
                          << "," << yp << "," << q << "\n";
                    }
                }
            }
        }
        double Percent = 100.0*(1.0*xp+1)/(1.0*Npix);
        cout << Percent << " Percent Completed" << endl;
    }
    qint_output.close();

    return;
}

double Qint::Tint(int n)
{
    alpha_n=(n + 0.5)*pi2;  //here it is alpha_n/2.
    double Integral_Value=0.; //Initializing the variable representing the integral result
    double Square_Root_X_Value = 0; //Initialize value of log(x)
                                    //[aka log(eta)] that will be computed many times
    for (int i=0; i<Nint; i++) {
        double x, y, erx, ery;
        x= (0.5 + i)/Nint; //eta in the formula (Eq. 10)
        Square_Root_X_Value = 1/sqrt(-log(x));
        erx  =erf(alpha_n*(b_i1-X0)*Square_Root_X_Value); //error function for x in integral
        erx -=erf(alpha_n*(b_i-X0)*Square_Root_X_Value); //error function for x in integral
        ery  =erf(alpha_n*(c_j1-Y0)*Square_Root_X_Value); //error function for y in integral
        ery -=erf(alpha_n*(c_j-Y0)*Square_Root_X_Value); //error function for y in integral
        y =erx*ery; //The value for the smaller component of the integral
        Integral_Value +=y; // adding each partition of integral
        //vx.push_back(x);
        //vy.push_back(y);
        //cout <<" i "<<i<<" x "<<xi<<" y "<<yi<<endl;
    }
    Integral_Value *= (1./Nint) * .25; //Returning the function value
    //Multiply by .25 because equation calls for it
    //cout<<" n "<<n<<" Integral "<<tin<<" for Nint "<<Nint<<endl;

    return Integral_Value;
}

int Qint::Tintegral ( void )
{

 // number of integration steps
    int const N=20;
    double tit[N], it[N];
    vector <double> x[N], y[N];
    for (int j=0; j<N; j++){
        Nint=20+j*10;
        it[j]=Nint;
        tit[j]= Tint(0);  //, x[j], y[j]);
    }

    // summation steps
    int const sumN=20;
    double stit[sumN], sit[sumN];
    vector <double> Sine[sumN], sy[sumN];
    for (int j=0; j<sumN; j++){
        sit[j]=j;
        stit[j]= Tint(j);
   }

    TCanvas    * CT = new TCanvas( "function", "function", 100, 70, 900, 800);
    CT->SetBorderMode  (0);      //to remove border
    CT->SetLeftMargin  (left_margin);   //to move plot to the right 0.14
    CT->SetRightMargin (right_margin);  //to move plot to the right 0.05
    CT->SetTopMargin   (top_margin);    //to use more space 0.07
    CT->SetBottomMargin(bot_margin);    //default
    CT->SetFrameFillColor(0);
    CT->Divide(2,2);

    gPad->SetFillStyle(4100);
    gPad->SetFillColor(0);

   CT->cd(1);
    unsigned int VNpnt = x[19].size();
    TGraph *gr= new TGraph(VNpnt,&x[19][0],&y[19][0]);
    //gr->Draw();
    gr->SetMarkerColor(4);
    gr->SetMarkerStyle(20);
    gr->SetMarkerSize(0.5);
    gr->SetTitle("integrand function n=0");
    gr->GetXaxis()->SetNdivisions(505);
    gr->GetXaxis()->CenterTitle();
    gr->GetXaxis()->SetTitle("eta");
    gr->GetXaxis()->SetLabelSize(0.04);
    gr->GetXaxis()->SetTitleSize(0.04);
    gr->GetYaxis()->SetNdivisions(505);
    gr->GetYaxis()->CenterTitle();
    gr->GetYaxis()->SetTitle("function (eta)");
    gr->GetYaxis()->SetLabelSize(0.04);
    gr->GetYaxis()->SetTitleSize(0.04);
    gr->GetYaxis()->SetTitleOffset(1.4);
    gr->Draw("ALP");

    CT->cd(2);
    TGraph *gr2= new TGraph(N,it,tit);
    gr2->SetMarkerColor(4);
    gr2->SetMarkerStyle(21);
    gr2->SetMarkerSize(0.5);
    gr2->SetTitle("0-th integral");
    gr2->GetXaxis()->SetNdivisions(505);
    gr2->GetXaxis()->CenterTitle();
    gr2->GetXaxis()->SetTitle("number of integration steps");
    gr2->GetXaxis()->SetLabelSize(0.04);
    gr2->GetXaxis()->SetTitleSize(0.04);
    gr2->GetYaxis()->SetNdivisions(505);
    gr2->GetYaxis()->CenterTitle();
    gr2->GetYaxis()->SetTitle("Integral value");
    gr2->GetYaxis()->SetLabelSize(0.04);
    gr2->GetYaxis()->SetTitleSize(0.04);
    gr2->GetYaxis()->SetTitleOffset(1.6);
    gr2->Draw("ALP");

    CT->cd(3);
     VNpnt = Sine[1].size();
     TGraph *gr3= new TGraph(VNpnt,&Sine[1][0],&sy[1][0]);
     //gr->Draw();
     gr3->SetMarkerColor(4);
     gr3->SetMarkerStyle(20);
     gr3->SetMarkerSize(0.5);
     gr3->SetTitle("integrand function n=1");
     gr3->GetXaxis()->SetNdivisions(505);
     gr3->GetXaxis()->CenterTitle();
     gr3->GetXaxis()->SetTitle("eta");
     gr3->GetXaxis()->SetLabelSize(0.04);
     gr3->GetXaxis()->SetTitleSize(0.04);
     gr3->GetYaxis()->SetNdivisions(505);
     gr3->GetYaxis()->CenterTitle();
     gr3->GetYaxis()->SetTitle("function (eta)");
     gr3->GetYaxis()->SetLabelSize(0.04);
     gr3->GetYaxis()->SetTitleSize(0.04);
     gr3->GetYaxis()->SetTitleOffset(1.4);
     gr3->Draw("ALP");

     CT->cd(4);
     TGraph *gr4= new TGraph(N,sit,stit);
    gr4->SetMarkerColor(4);
    gr4->SetMarkerStyle(20);
    gr4->SetMarkerSize(0.5);
    gr4->SetTitle("n-th integral");
    gr4->GetXaxis()->SetNdivisions(505);
    gr4->GetXaxis()->CenterTitle();
    gr4->GetXaxis()->SetTitle("summation step");
    gr4->GetXaxis()->SetLabelSize(0.04);
    gr4->GetXaxis()->SetTitleSize(0.04);
    gr4->GetYaxis()->SetNdivisions(505);
    gr4->GetYaxis()->CenterTitle();
    gr4->GetYaxis()->SetTitle("integral value");
    gr4->GetYaxis()->SetLabelSize(0.04);
    gr4->GetYaxis()->SetTitleSize(0.04);
    gr4->GetYaxis()->SetTitleOffset(1.4);
    gr4->Draw("ALP");

    CT->Update();

    return 0.;
}

int Qint::Sinx ( double zz )
{
    const double pii(TMath::Pi());
    const int  N=5000;
    const double H=100.;
    vector <double> Sine, sy1, sy2;
    for (int i=0; i<N; i++){
        double a=(i+0.5)*H/N;
        double b=sin(a*zz)/a;
        Sine.push_back(a/pii);
        sy1.push_back(b);
        double d=sin(a*0.1)/a;
        sy2.push_back(d);
    }
    TCanvas    * CS = new TCanvas( "Sinx", "Sinx", 100, 70, 900, 800);
    CS->SetBorderMode  (0);      //to remove border
    CS->SetLeftMargin  (left_margin);   //to move plot to the right 0.14
    CS->SetRightMargin (right_margin);  //to move plot to the right 0.05
    CS->SetTopMargin   (top_margin);    //to use more space 0.07
    CS->SetBottomMargin(bot_margin);    //default
    CS->SetFrameFillColor(0);
    CS->Divide(1,1);

    gPad->SetFillStyle(4100);
    gPad->SetFillColor(0);

   CS->cd(1);
    unsigned int VNpnt = Sine.size();
    TGraph *gr= new TGraph(VNpnt,&Sine[0],&sy1[0]);
    gr->Draw();
    TGraph *gr2= new TGraph(VNpnt,&Sine[0],&sy2[0]);
    gr2->Draw("same");

    return 0;
}

double Qint::Qj(void)   //double x0, double y0, double z0)
{
    bool in_pixel=FALSE; //If its in the pixel
    if ((X0>b_i)&&(X0<b_i1) &&
        (Y0>c_j)&&(Y0<c_j1) ) {in_pixel=TRUE;}
        //Setting a variable to True if the coordinates are within the central pixel
    double Sine=0.0; //Sine term in the loop
    double I_ij_n=0.0; //Variable to store integral value for each index of sum (I_ij)
    double term=0.0;  //term is just each sum term, multiplication of the integral and the sine


    const int N=500; //Number of summations

    //double sum=0.0;
/*    for (int i_sum=0; i_sum<N; i_sum++){
        alpha_n=(i_sum+0.5)*pii;
        Sine=sin(alpha_n*Z0)/alpha_n;
        I_ij_n= Tint(i_sum);
        term=2.*Sine*I_ij_n;
        sum+=term;
        //if ( i_sum % 2 == 0) {  // i_sum is even
            s_idx.push_back(i_sum);
            vsum.push_back(sum);
        //}
     }
 */
    double Sum_Value = 0.0;  //Sum
    bool lstop=FALSE;  //Variable that tells the summation to stop if the integral is less than valid
    int Nprd=(int)(2./Z0); //Period of the sine oscillation
    int Nsum=0; //Index to keep track of number of iterations of summation
    for (int i_sum=0; i_sum<N; i_sum++){
        Nsum++; //Index
        alpha_n=(i_sum+0.5)*pii; //Compute alpha_n
        Sine=sin(alpha_n*Z0)/alpha_n;  //Compute sine value
        I_ij_n=Tint(i_sum); //Compute integral
        if (in_pixel) {I_ij_n= 1.-I_ij_n;}
        term=Sine*I_ij_n; //Multiplying the terms in the summation
        Sum_Value+=term; //Adding the term to the summation
        //cout<<"i="<<i_sum<<"alpha_n="<<alpha_n<<"Sine="<<Sine<<"I_ij_n="<<I_ij_n<<"term="<<term<<endl;
        if (I_ij_n<Valid) {lstop=TRUE;}  // break;} //If the integral is less than the cutoff, prepare to stop summation
        if ( ( (i_sum+1)%Nprd == 0) && lstop ) {break;}
     }
    Sum_Value = 2 * Sum_Value; //Doubling it as it says in the equation
    if (in_pixel) {Sum_Value =1.-Sum_Value;}
    //cout<<"* stop at "<<Nsum<<" Sum_Value "<<Sum_Value<<endl;
    //<<" long sum "<<sum
    //cout<<"X0="<<X0<<"Sine="<<Sine<<"I_ij_n="<<I_ij_n<<"term="<<term<<endl;

    return Sum_Value;  //Nsum;
}
int Qint::PlotQj(void)   //double x0, double y0, double z0)
{
    TCanvas    * CQ = new TCanvas( "qj", "qj", 120, 80, 900, 800);
    CQ->SetBorderMode  (0);      //to remove border
    CQ->SetLeftMargin  (left_margin);   //to move plot to the right 0.14
    CQ->SetRightMargin (right_margin);  //to move plot to the right 0.05
    CQ->SetTopMargin   (top_margin);    //to use more space 0.07
    CQ->SetBottomMargin(bot_margin);    //default
    CQ->SetFrameFillColor(0);
    CQ->Divide(1,1);

    gPad->SetFillStyle(4100);
    gPad->SetFillColor(0);

//    CQ->cd(1);
//    unsigned int VNpnt = s_idx.size();
//    TGraph *grq= new TGraph(VNpnt,&s_idx[0],&vsum[0]);
//    grq->Draw();

    CQ->cd(1);
    StermN->GetXaxis()->SetNdivisions(505);
    StermN->GetXaxis()->CenterTitle();
    StermN->GetXaxis()->SetTitle("X coordinate");
    StermN->GetXaxis()->SetLabelSize(0.05);
    StermN->GetXaxis()->SetTitleSize(0.05);
    StermN->GetYaxis()->SetNdivisions(505);
    StermN->GetYaxis()->CenterTitle();
    StermN->GetYaxis()->SetTitle("Y coordinate");
    StermN->GetYaxis()->SetLabelSize(0.05);
    StermN->GetYaxis()->SetTitleSize(0.05);
    //StermN->Draw("lego");
    StermN->Draw("colz");

    return 0;
}

int Qint::PlotProf(void)
{
    TCanvas    * CP = new TCanvas( "Profile", "PROFILE", 125, 80, 900, 800);
    CP->SetBorderMode  (0);      //to remove border
    CP->SetLeftMargin  (left_margin);   //to move plot to the right 0.14
    CP->SetRightMargin (right_margin);  //to move plot to the right 0.05
    CP->SetTopMargin   (top_margin);    //to use more space 0.07
    CP->SetBottomMargin(bot_margin);    //default
    CP->SetFrameFillColor(0);
    CP->Divide(1,1);

    gPad->SetFillStyle(4100);
    gPad->SetFillColor(0);

    Qtot=0.;
    for (int y=0; y<Npix; y++){
        c_j=(y-Npix/2.)*PIX_WDTH;
        c_j1=c_j+PIX_WDTH;
        for (int x=0; x<Npix; x++){
            b_i=(x-Npix/2.)*PIX_WDTH;
            b_i1=b_i+PIX_WDTH;
            double amp=Qj();
            hProf->SetBinContent(x+1,y+1,amp);
            Qtot +=amp;
            //cout<<"x"<<x<<"y"<<y<<"N"<<Ntrm<<endl;
        }
    }
    cout<<" Qtot "<<Qtot<<endl;

    CP->cd(1);
    hProf->GetXaxis()->SetNdivisions(505);
    hProf->GetXaxis()->CenterTitle();
    hProf->GetXaxis()->SetTitle("X coordinate");
    hProf->GetXaxis()->SetLabelSize(0.05);
    hProf->GetXaxis()->SetTitleSize(0.05);
    hProf->GetYaxis()->SetNdivisions(505);
    hProf->GetYaxis()->CenterTitle();
    hProf->GetYaxis()->SetTitle("Y coordinate");
    hProf->GetYaxis()->SetLabelSize(0.05);
    hProf->GetYaxis()->SetTitleSize(0.05);
    hProf->Draw("lego");
    //hProf->Draw("colz");

    return 0;

}

int Qint_main ( double xx, double yy, double zz)
{
    auto start = std::chrono::steady_clock::now(); //Starting Clock
// clean up previous histograms
    gDirectory->GetList()->Delete();

    Qint Qi(xx, yy, zz);
    Qi.PlotProf();
    //Qi.Tintegral();
    //Qi.Sinx(zz);
/*    double xstp=3./Qint::NgX;  //Qint::B
    double ystp=1./Qint::NgY;  //Qint::E
    for (int y=0; y<Qint::NgY; y++){
        Qi.Y0=(y+0.5)*ystp;  //(y+0.5)
        for (int x=0; x<Qint::NgX; x++){
            Qi.X0=(x+0.5)*xstp;  //(x+0.5)
            int Ntrm=Qi.Qj();
            Qi.StermN->SetBinContent(x+1,y+1,Ntrm);
            //cout<<"x"<<x<<"y"<<y<<"N"<<Ntrm<<endl;
        }
    }
    //Qi.Qj();
    Qi.PlotQj();
*/
    auto end = std::chrono::steady_clock::now(); //End Clock
    double elapsed_time = double(std::chrono::duration_cast <std::chrono::nanoseconds>(end - start).count()); //Subtract Time
    cout << "Elapsed Time (s)" << elapsed_time / 1e9 << endl; //Printing Time
    return 0;
}

#endif //__CINT__
