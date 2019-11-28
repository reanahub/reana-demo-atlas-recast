FROM atlas/analysisbase:21.2.51
ADD . /analysis/src
WORKDIR /analysis/build
RUN source ~/release_setup.sh &&  \
    sudo chown -R atlas /analysis && \
    cmake ../src && \
    make -j4
USER root
RUN sudo usermod -G root atlas
USER atlas
