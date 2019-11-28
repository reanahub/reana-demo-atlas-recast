FROM atlas/analysisbase:21.2.51
ADD . /code
RUN sudo sh -c "source /home/atlas/release_setup.sh && pip install hftools==0.0.6"
USER root
RUN sudo usermod -G root atlas
USER atlas
