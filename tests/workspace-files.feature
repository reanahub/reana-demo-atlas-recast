# Tests for the presence of files in the workspace

Feature: Workspace files

    As a researcher,
    I want to make sure that my workflow produces the expected files,
    and that they are not corrupted, so that I can
    be sure that the workflow outputs are correct.

    Scenario: The output plot is correctly generated
        When the workflow is finished
        Then the workspace should contain "statanalysis/fitresults/limit.png"
        And the "sha256" checksum of the file "statanalysis/fitresults/limit.png" should be
            """
            5661f3f55d664715c8b84a5ffee2b4d6fd5d9265971629c2e16f013abef09ee2
            """
