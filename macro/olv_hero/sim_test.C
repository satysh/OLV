// ------------------------------------------------------------------------
//  ===== STANDARD ION TABLE (Z, A) =====
//
//      H  :  1,  1       Si : 14, 28
//      He :  2,  4       P  : 15, 31
//      Li :  3,  7       S  : 16, 32
//      Be :  4,  9       Cl : 17, 35
//      B  :  5, 11       Ar : 18, 40
//      C  :  6, 12       K  : 19, 39
//      N  :  7, 14       Ca : 20, 40
//      O  :  8, 16       Sc : 21, 45
//      F  :  9, 19       Ti : 22, 48
//      Ne : 10, 20        V : 23, 51
//      Na : 11, 23       Cr : 24, 52
//      Mg : 12, 24       Mn : 25, 55
//      Al : 13, 27       Fe : 26, 56
// ------------------------------------------------------------------------

int  GetPdgCode(const int Z, const int A);
void AddIon(const int pdg);                    //For PDG ion beam

void sim_test(Int_t nEvents = 1, Int_t index = 0, TString outDir = "output")
{

  // -----   Particle  --------------------------------------------------------
  Int_t pdgId = 2112; // proton 2212 // electron 11
  Double32_t momentum = 1000.; // GeV


  pdgId = 2212;
  //pdgId = GetPdgCode(82,207);      // Set nuclear pdg for Ion

  // ------------------------------------------------------------------------


  gRandom->SetSeed(index);

  //---------------------Files-----------------------------------------------
  TString outFile = outDir + "/sim.root";
  TString parFile = outDir + "/par.root";
  // ------------------------------------------------------------------------

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();

  FairRunSim* run = new FairRunSim();
  run->SetName("TGeant4");              // Transport engine
  run->SetOutputFile(outFile.Data());          // Output file
  run->SetMaterials("media.geo");       // Materials

  FairModule* cave= new FairCave("CAVE");
  cave->SetGeometryFileName("cave.geo");
  run->AddModule(cave);

  HERODetector* detector = new HERODetector("HEROdetector", kTRUE);
  detector->SetGeometryFileName("HERO_detector.geo.root");
  detector->AddSensetive("vCub");
  run->AddModule(detector);

// -----   Create PrimaryGenerator   --------------------------------------
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
  FairBoxGenerator* boxGen = new FairBoxGenerator(pdgId, 1);
  boxGen->SetEkinRange(momentum, momentum);
  boxGen->SetThetaRange(0., 0.); // 0-90
  boxGen->SetPhiRange(0., 0.); // 0-360
  boxGen->SetBoxXYZ(0., 0., 0., 0., -500.); // xmin, ymin, xmax, ymax, z
  // ------------------------------------------------------------------------
  //AddIon(pdgId);
  // ------------------------------------------------------------------------

  primGen->AddGenerator(boxGen);
  run->SetGenerator(primGen);

  run->SetStoreTraj(kTRUE); // kFALSE

  //-------Set LOG verbosity  -----------------------------------------------
  FairLogger::GetLogger()->SetLogVerbosityLevel("LOW");
  FairLogger::GetLogger()->SetLogScreenLevel("DEBUG");

  run->Init();

  // -----   Runtime database   ---------------------------------------------
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  parOut->open(parFile.Data());
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();

  // -----   Run simulation  ------------------------------------------------
  run->Run(nEvents);

  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is sim.root" << endl;
  cout << "Parameter file is par.root" << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime
                  << "s" << endl << endl;
}
int GetPdgCode(const int Z, const int A)             //For PDG ion beam
{
  if (Z == 1 && A == 1) return 2212;
  return 1000000000 + Z*10000 + A*10;
}
void AddIon(const int pdg)
{
  if (pdg < 1000000000) return;

  FairRunSim* run = FairRunSim::Instance();
  if (!run) return;

  int Z = (pdg-1000000000)/10000;
  int A = (pdg-1000000000-10000*Z)/10;

  FairIon* ion = new FairIon();
  ion->SetParams(Form("ION_%03d_%03d",Z,A), Z, A, Z);

  run->AddNewIon(ion);
}

