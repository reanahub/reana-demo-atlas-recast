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
first and its output serve as an input for the statistical analysis. results.

We shall use the `Yadage <https://github.com/yadage>`_ workflow engine to
express the computational steps in a declarative manner. The `workflow.yml
<workflow/workflow.yml>`_ workflow defines the full pipeline.

.. code-block:: text

         inputs
           |
           |
           V
   +-----------------+
   | Event selection |
   +-----------------+
           |
           |  submitDir/input/sample.root
           |  submitDir/hist-sample.root
           |  submitDirhist/sample.root
           |  submitDir/driver.root
           |
           V
   +----------------------+
   | Statistical analysis |
   +----------------------+
           |
           |  fitresults/pre.png
           |  fitresults/limit.png
           |  fitresults/limit_data.json
           |  fitresults/post.png
           |  fitresults/limit_data_nomsignal.json
           |
           V
         outputs

Please see the `workflow.yml <workflow/workflow.yml>`_ workflow definition with
individual stages being defined in `steps.yml <workflow/steps.yml>`_ and refer
to `Yadage documentation <http://yadage.readthedocs.io/>`_.

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

.. figure:: https://raw.githubusercontent.com/reanahub/reana-demo-atlas-recast/master/statanalysis/example_results/plot_limit.png
   :alt: plot_limit.png
   :align: center

The limit data is also stored in JSON format for both an entire µ-scan as well
as for µ=1.

Local testing
=============

*Optional*

If you would like to test the analysis locally (i.e. outside of the REANA
platform), you can proceed as follows:

.. code-block:: console

   $ # this analysis example uses yadage; let's install it
   $ mkvirtualenv yadage
   $ pip install yadage==0.13.5 yadage-schemas==0.7.16 packtivity==0.10.0
   $ # we can now run the analysis workflow
   $ export PACKTIVITY_DOCKER_CMD_MOD="-u root"
   $ yadage-run _run workflow/workflow.yml ./workflow/test_inputs/inp1.yml
   $ # let us check output files
   $ ls -l _run/statanalysis/fitresults/*.png
   -rw-r--r-- 1 root root 17053 Jun 17 15:16 _run/statanalysis/fitresults/limit.png
   -rw-r--r-- 1 root root 10120 Jun 17 15:16 _run/statanalysis/fitresults/post.png
   -rw-r--r-- 1 root root 10142 Jun 17 15:16 _run/statanalysis/fitresults/pre.png
   $ ls -l _run/statanalysis/fitresults/limit_data.json
   -rw-r--r-- 1 root root 174 Jun 17 15:16 _run/statanalysis/fitresults/limit_data.json

Running the example on REANA cloud
==================================

First we need to create a `reana.yaml <reana.yaml>`_ file describing the
structure of our analysis with its inputs, the code, the runtime environment,
the workflow and the expected outputs:

.. code-block:: yaml

    version: 0.2.0
    inputs:
      parameters:
        did: 404958
        xsec_in_pb: 0.00122
        dxaod_file: http://physics.nyu.edu/~lh1132/capdemo/mc15_13TeV.123456.cap_recast_demo_signal_one.root
    outputs:
      files:
        - outputs/statanalysis/fitresults/pre.png
        - outputs/statanalysis/fitresults/post.png
        - outputs/statanalysis/fitresults/limit.png
        - outputs/statanalysis/fitresults/limit_data.json
    environments:
      - type: docker
        image: reanahub/reana-demo-atlas-recast-eventselection
      - type: docker
        image: reanahub/reana-demo-atlas-recast-statanalysis
    workflow:
      type: yadage
      file: workflow/workflow.yml

We proceed by installing the REANA command-line client:

.. code-block:: console

    $ mkvirtualenv reana-client
    $ pip install reana-client

We should now connect the client to the remote REANA cloud where the analysis
will run. We do this by setting the ``REANA_SERVER_URL`` environment variable:

.. code-block:: console

    $ export REANA_SERVER_URL=https://reana.cern.ch/

Note that if you `run REANA cluster locally
<http://reana-cluster.readthedocs.io/en/latest/gettingstarted.html#deploy-reana-cluster-locally>`_
on your laptop, you would do:

.. code-block:: console

    $ eval $(reana-cluster env)

Let us test the client-to-server connection:

.. code-block:: console

    $ reana-client ping
    Server is running.

We proceed to create a new workflow instance:

.. code-block:: console

    $ reana-client workflow create
    workflow.1
    $ export REANA_WORKON=workflow.1

We can now start the workflow execution:

.. code-block:: console

    $ reana-client workflow start
    workflow.1 has been started.

After several minutes the workflow should be successfully finished. Let us query
its status:

.. code-block:: console

    $ reana-client workflow status
    NAME       RUN_NUMBER   CREATED               STATUS     PROGRESS
    workflow   1            2018-06-20T18:29:36   finished   2/2

We can list the output files:

.. code-block:: console

    $ reana-client outputs list | grep -E '(NAME|fitresults)'
    NAME                                                SIZE    LAST-MODIFIED
    statanalysis/fitresults/pre.png                     10142   2018-06-20 18:31:17.435274+00:00
    statanalysis/fitresults/post.png                    10120   2018-06-20 18:31:17.435274+00:00
    statanalysis/fitresults/limit.png                   17053   2018-06-20 18:31:17.435274+00:00
    statanalysis/fitresults/limit_data.json             174     2018-06-20 18:31:17.435274+00:00
    statanalysis/fitresults/limit_data_nomsignal.json   176     2018-06-20 18:31:17.435274+00:00

We finish by downloading generated limit plot:

.. code-block:: console

    $ reana-client outputs download statanalysis/fitresults/limit.png
    File statanalysis/fitresults/limit.png downloaded to ./outputs/

Contributors
============

The list of contributors in alphabetical order:

- `Lukas Heinrich <https://orcid.org/0000-0002-4048-7584>`_ <lukas.heinrich@gmail.com>
- `Tibor Simko <https://orcid.org/0000-0001-7202-5803>`_ <tibor.simko@cern.ch>
