==============================
 REANA example - ATLAS RECAST
==============================

.. image:: https://img.shields.io/travis/reanahub/reana-demo-atlas-recast.svg
   :target: https://travis-ci.org/reanahub/reana-demo-atlas-recast

.. image:: https://badges.gitter.im/Join%20Chat.svg
   :target: https://gitter.im/reanahub/reana?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge

.. image:: https://img.shields.io/github/license/reanahub/reana-demo-atlas-recast.svg
   :target: https://raw.githubusercontent.com/reanahub/reana-demo-atlas-recast/master/LICENSE

About
=====

This `REANA <http://www.reana.io/>`_ reproducible analysis example demonstrates
a `RECAST <https://arxiv.org/abs/1010.2506>`_ analysis using `ATLAS
<https://atlas.cern/>`_ Analysis Software Group stack.

Analysis structure
==================

Making a research data analysis reproducible basically means to provide
"runnable recipes" addressing (1) where is the input data, (2) what software was
used to analyse the data, (3) which computing environments were used to run the
software and (4) which computational workflow steps were taken to run the
analysis. This will permit to instantiate the analysis on the computational
cloud and run the analysis to obtain (5) output results.

1. Input data
-------------

The analysis takes the following inputs:

- ``dxaod`` input ROOT file
- ``did`` dataset ID e.g. 404958
- ``xsec_in_pb`` cross section in picobarn e.g. 0.00122

2. Analysis code
----------------

The **event selection** code for this analysis example resides under the
`eventselection <eventselection>`_ subdirectory. It uses the official analysis
releases prepared by the ATLAS Analysis Software Group (ASG).

- ``eventselection/CMakeLists.txt``
- ``eventselection/MyEventSelection/CMakeLists.txt``
- ``eventselection/MyEventSelection/MyEventSelection/MyEventSelectionAlg.h``
- ``eventselection/MyEventSelection/Root/LinkDef.h``
- ``eventselection/MyEventSelection/Root/MyEventSelectionAlg.cxx``
- ``eventselection/MyEventSelection/util/myEventSelection.cxx``

The **statistical analysis** code for this analysis example resides in
`statanalysis <statanalysis>`_ subdirectory. It implements limit setting for
outputs produced by the event selection package.

- ``statanalysis/data/background.root``
- ``statanalysis/data/data.root``
- ``statanalysis/make_ws.py``
- ``statanalysis/plot.py``
- ``statanalysis/set_limit.py``

Notes that ``make_ws.py`` script generates a HistFactory configuration based on
signal, data and background ROOT files. It performs a simple HistFactory-based
fit based on a single channel (consisting of two bins).

3. Compute environment
----------------------

In order to be able to rerun the analysis even several years in the future, we
need to "encapsulate the current compute environment", for example to freeze the
ATLAS software version our analysis is using. We shall achieve this by preparing
a `Docker <https://www.docker.com/>`_ container image for our analysis steps.

The event selection stage uses official ATLAS ``atlas/analysisbase`` container
on top of which we add and build our custom code:

.. code-block:: console

   $ less eventselection/Dockerfile
   FROM atlas/analysisbase:latest
   ADD . /analysis/src
   WORKDIR /analysis/build
   RUN source ~/release_setup.sh &&  \
       sudo chown -R atlas /analysis && \
       cmake ../src && \
       make -j4

We can build our event selection analysis environment image and give it a name
``reanahub/reana-demo-atlas-recast-eventselection``:

.. code-block:: console

   $ cd eventselection
   $ docker build -t reanahub/reana-demo-atlas-recast-eventselection .

The statistical analysis stage also extends ``atlas/analysisbase`` by the custom
code:

.. code-block:: console

   $ less statanalysis/Dockerfile
   FROM atlas/analysisbase
   ADD . /code
   RUN sudo sh -c "source /home/atlas/release_setup.sh && pip install hftools"

We can build our statistical analysis environment image and give it a name
``reanahub/reana-demo-atlas-recast-statanalysis``:

.. code-block:: console

   $ cd statanalysis
   $ docker build -t reanahub/reana-demo-atlas-recast-statanalysis .

We can upload both images to the DockerHub image registry:

.. code-block:: console

   $ docker push reanahub/reana-demo-atlas-recast-eventselection
   $ docker push reanahub/reana-demo-atlas-recast-statanalysis

(Note that typically you would use your own username such as ``johndoe`` in
place of ``reanahub``.)

4. Analysis workflow
--------------------

This analysis example consists of a simple workflow where event selection is run
first and its output serve as an input for the statistical analysis.

We shall use the `Yadage <https://github.com/yadage>`_ workflow engine to
express the computational steps in a declarative manner:

.. figure:: https://raw.githubusercontent.com/reanahub/reana-demo-atlas-recast/master/docs/workflow.png
   :alt: workflow.png
   :align: center

The full analysis pipeline is defined in `workflow.yml <workflow/workflow.yml>`_
and the individual steps are defined in `steps.yml <workflow/steps.yml>`_.

5. Output results
-----------------

The analysis produces several pre-fit and post-fit plots:

.. figure:: https://raw.githubusercontent.com/reanahub/reana-demo-atlas-recast/master/statanalysis/example_results/pre.png
   :alt: pre.png
   :align: center

.. figure:: https://raw.githubusercontent.com/reanahub/reana-demo-atlas-recast/master/statanalysis/example_results/post.png
   :alt: post.png
   :align: center

The limit plot:

.. figure:: https://raw.githubusercontent.com/reanahub/reana-demo-atlas-recast/master/statanalysis/example_results/limit.png
   :alt: limit.png
   :align: center

The limit data is also stored in JSON format for both an entire µ-scan as well
as for µ=1.

Running the example on REANA cloud
==================================

We start by creating a `reana.yaml <reana.yaml>`_ file describing the above
analysis structure with its inputs, code, runtime environment, computational
workflow steps and expected outputs:

.. code-block:: yaml

    version: 0.3.0
    inputs:
      parameters:
        did: 404958
        xsec_in_pb: 0.00122
        dxaod_file: https://recastwww.web.cern.ch/recastwww/data/reana-recast-demo/mc15_13TeV.123456.cap_recast_demo_signal_one.root
    workflow:
      type: yadage
      file: workflow/workflow.yml
    outputs:
      files:
        - outputs/statanalysis/fitresults/pre.png
        - outputs/statanalysis/fitresults/post.png
        - outputs/statanalysis/fitresults/limit.png
        - outputs/statanalysis/fitresults/limit_data.json

We can now install the REANA command-line client, run the analysis and download
the resulting plots:

.. code-block:: console

    $ # create new virtual environment
    $ virtualenv ~/.virtualenvs/myreana
    $ source ~/.virtualenvs/myreana/bin/activate
    $ # install REANA client
    $ pip install reana-client
    $ # connect to some REANA cloud instance
    $ export REANA_SERVER_URL=https://reana.cern.ch/
    $ export REANA_ACCESS_TOKEN=XXXXXXX
    $ # create new workflow
    $ reana-client create -n my-analysis
    $ export REANA_WORKON=my-analysis
    $ # upload input code and data to the workspace
    $ reana-client upload ./code ./data
    $ # start computational workflow
    $ reana-client start
    $ # ... should be finished in about a minute
    $ reana-client status
    $ # list workspace files
    $ reana-client list
    $ # download output results
    $ reana-client download statanalysis/fitresults/limit.png

Please see the `REANA-Client <https://reana-client.readthedocs.io/>`_
documentation for more detailed explanation of typical ``reana-client`` usage
scenarios.

Contributors
============

The list of contributors in alphabetical order:

- `Diego Rodriguez <https://orcid.org/0000-0003-0649-2002>`_
- `Lukas Heinrich <https://orcid.org/0000-0002-4048-7584>`_
- `Rokas Maciulaitis <https://orcid.org/0000-0003-1064-6967>`_
- `Tibor Simko <https://orcid.org/0000-0001-7202-5803>`_
