FROM atlas/analysisbase
ADD . /code
RUN sudo sh -c "source /home/atlas/release_setup.sh && pip install hftools"
USER root
RUN sudo usermod -G root atlas
USER atlas
