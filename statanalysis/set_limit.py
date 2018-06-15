import ROOT
import ROOT.RooStats
import json
import sys

def main():

    workspace_file = sys.argv[1]
    plotfile = sys.argv[2]
    resultsfile = sys.argv[3]
    resultsfile_mu_one = sys.argv[4]

    f = ROOT.TFile.Open(workspace_file)
    ws = f.Get('combined')


    sbmodel = ws.obj('ModelConfig')
    sbmodel.SetName('sbmodel')
    sbmodel.SetSnapshot(ROOT.RooArgSet(sbmodel.GetParametersOfInterest().first()))

    bmodel = sbmodel.Clone()
    bmodel.SetName('bmodel')
    bmodel.GetParametersOfInterest().first().setVal(0)
    bmodel.SetSnapshot(ROOT.RooArgSet(bmodel.GetParametersOfInterest().first()))
    bmodel.GetParametersOfInterest().first().setVal(1)


    calc = ROOT.RooStats.AsymptoticCalculator(ws.data('obsData'),bmodel,sbmodel,False)
    calc.SetOneSided(True)


    inverter = ROOT.RooStats.HypoTestInverter(calc)
    inverter.UseCLs(True)
    inverter.SetConfidenceLevel(0.95)
    inverter.SetFixedScan(21,0,2.0)
    result = inverter.GetInterval()


    plot = ROOT.RooStats.HypoTestInverterPlot("HTI_Result_Plot",'result',result)


    c = ROOT.TCanvas()
    plot.Draw()
    c.SaveAs(plotfile)

    resultdata = {
    'exp_m2':result.GetExpectedUpperLimit(-2),
    'exp_m1':result.GetExpectedUpperLimit(-1),
    'exp':result.GetExpectedUpperLimit(0),
    'exp_p1':result.GetExpectedUpperLimit(1),
    'exp_p2':result.GetExpectedUpperLimit(2),
    'obs': result.UpperLimit()
    }
    json.dump(resultdata,open(resultsfile,'w'))


    mu_index = 5
    mu_one = result.GetResult(mu_index)
    print result.GetXValue(mu_index),result.GetYValue(mu_index)
    import numpy as np
    sampling = list(result.GetExpectedPValueDist(mu_index).GetSamplingDistribution())
    mu_one_data = dict(zip(['exp_m2','exp_m1','exp','exp_p1','exp_p2','obs'],sampling[3:8]+[result.GetYValue(mu_index)]))

    json.dump(mu_one_data,open(resultsfile_mu_one,'w'))

if __name__ == '__main__':
    main()
