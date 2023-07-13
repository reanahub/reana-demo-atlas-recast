FROM docker.io/atlas/analysisbase:21.2.51
ADD . /code
RUN sudo sh -c "source /home/atlas/release_setup.sh && pip install hftools==0.0.6"
USER root
RUN sudo usermod -G root atlas && sudo chmod -R 755 /home/atlas && chmod -R 775 /code
USER atlas
