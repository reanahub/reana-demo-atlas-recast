import sys
import ROOT
import ROOT.RooStats

def main():
    data_file       = sys.argv[1]
    signal_file     = sys.argv[2]
    background_file = sys.argv[3]

    meas = ROOT.RooStats.HistFactory.Measurement("meas", "meas")

    meas.SetOutputFilePrefix("./results/meas")
    meas.SetPOI("SigXsecOverSM")
    meas.AddConstantParam("Lumi")

    meas.SetLumi(1.0)
    meas.SetLumiRelErr(0.10)
    meas.SetExportOnly(True)

    # Create a channel

    chan = ROOT.RooStats.HistFactory.Channel("channel1")
    chan.SetData("data", data_file)
    chan.SetStatErrorConfig(0.05, "Poisson")

    # Now, create some samples
    signal = ROOT.RooStats.HistFactory.Sample("signal", "regions", signal_file)
    signal.AddOverallSys("syst1",  0.95, 1.05)
    signal.AddNormFactor("SigXsecOverSM", 1, 0, 3)
    chan.AddSample(signal)

    background1 = ROOT.RooStats.HistFactory.Sample("background", "background", background_file )
    chan.AddSample(background1)

    meas.AddChannel(chan)
    meas.PrintXML('config')

    meas.CollectHistograms();
    meas.PrintTree();

    ROOT.RooStats.HistFactory.MakeModelAndMeasurementFast(meas);


if __name__ == '__main__':
    main()
