#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include "CLD.h"

using namespace std;

int main(int argc, char **argv) {
  string filename;
  string output_filename("fdog.png");
  int etf_iterations = 3;
  int fdog_iterations = 3;
  
  CLD cld;

  cld.sigma_m = 5.0;
  cld.sigma_c = 1.0;
  cld.rho = 0.980;
  cld.tau = 0.980;

  char n;
  while ((n = getopt(argc, argv, "s:r:t:e:i:o:")) != -1) {
    switch(n) {
    case 's':
      cld.sigma_m = strtod(optarg, NULL);
      break;
    case 'r':
      cld.rho = strtod(optarg, NULL);
      break;
    case 't':
      cld.tau = strtod(optarg, NULL);
      break;
    case 'e':
      etf_iterations = (int)strtol(optarg, NULL, 10);
      break;
    case 'i':
      fdog_iterations = (int)strtol(optarg, NULL, 10);
      break;
    case 'o':
      output_filename = string(optarg);
      break;
    default:
      cout << "Not sure what this is: " << n << " (option " << optind << ")\n";
      return 1;
    }
  }

  if (optind < argc) {
    filename = argv[optind];
  }

  cerr << "Paramters:\n"
       << " Sigma (m): " << cld.sigma_m << endl
       << " Rho: " << cld.rho << endl
       << " Tau: " << cld.tau << endl
       << " ETF Iterations: " << etf_iterations << endl
       << " FDoG Iterations: " << fdog_iterations << endl
       << " File: " << filename << endl
       << " Save to: " << output_filename << endl
       << endl;
  
  cld.readSrc(filename);

  for(int i=0; i<etf_iterations; i++) {
    cout << "\rETF  " << i+1 << "/" << etf_iterations << flush;
    cld.etf.refine_ETF(7);
  }
  cout << endl;

  cld.genCLD();
  for(int i=0; i<fdog_iterations; i++) {
    cout << "\rFDoG iteration  " << i+1 << "/" << fdog_iterations << flush;
    cld.combineImage();
    cld.genCLD();
  }
  cout << endl;
  
  cv::imwrite(output_filename, cld.result);
  
  return 0;
}
