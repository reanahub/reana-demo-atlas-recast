FROM atlas/analysisbase:latest
ADD . /analysis/src
WORKDIR /analysis/build
RUN source ~/release_setup.sh &&  \
    sudo chown -R atlas /analysis && \
    cmake ../src && \
    make -j4

