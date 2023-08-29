# Tests for the expected workflow log messages

Feature: Log messages

    As a researcher,
    I want to be able to see the log messages of my workflow execution,
    So that I can verify that the workflow ran correctly.

    Scenario: The workflow start has produced the expected messages
        When the workflow is finished
        Then the engine logs should contain "yadage.creators | MainThread | INFO | initializing workflow"
        And the engine logs should contain "yadage.wflowview | MainThread | INFO | added </eventselection:0|defined|unknown>"
        And the engine logs should contain "yadage.wflowview | MainThread | INFO | added </statanalysis:0|defined|unknown>"

    Scenario: The event selection step has produced the expected messages
        When the workflow is finished
        Then the engine logs should contain "adage.pollingexec | MainThread | INFO | submitting nodes [</eventselection:0|defined|known>]"
        And the job logs for the "eventselection" step should contain
            """
            Running sample: sample
            Processing File: https://recastwww.web.cern.ch/recastwww/data/reana-recast-demo/mc15_13TeV.123456.cap_recast_demo_signal_one.root
            MyEventSelectionAlg::h... INFO    xsecfile recast_xsecs.txt
            MyEventSelectionAlg::h... INFO    xsec for 404958 is 0.001220
            """

    Scenario: The statistical analysis step has produced the expected messages
        When the workflow is finished
        Then the engine logs should contain "adage.pollingexec | MainThread | INFO | submitting nodes [</statanalysis:0|defined|known>]"
        And the job logs for the "statanalysis" step should contain "MIGRAD MINIMIZATION HAS CONVERGED."

    Scenario: The workflow completion has produced the expected messages
        When the workflow is finished
        Then the engine logs should contain "adage | MainThread | INFO | workflow completed successfully."
        And the engine logs should contain "adage | MainThread | INFO | execution valid. (in terms of execution order)"