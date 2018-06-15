import ROOT
import hftools.plotting
import sys

def main():
    filename = sys.argv[1]
    preplot = sys.argv[2]
    postplot = sys.argv[3]

    f = ROOT.TFile.Open(filename)
    ws = f.Get('combined')

    hftools.plotting.quickplot(
        ws,
        'channel1',
        'x',
        ['background','signal'],
        filename = preplot,
        title = 'Event Regions (pre-fit)',
        xaxis = 'region',
        yaxis = 'Events',
        singlebin = False,
        dimensions='600x600',
        logy = False
    )


    ws.pdf('simPdf').fitTo(ws.data('obsData'))

    hftools.plotting.quickplot(
        ws,
        'channel1',
        'x',
        ['background','signal'],
        filename = postplot,
        title = 'Event Regions (post-fit)',
        xaxis = 'region',
        yaxis = 'Events',
        singlebin = False,
        dimensions='600x600',
        logy = False
    )

if __name__ == '__main__':
    main()
