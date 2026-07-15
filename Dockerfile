FROM archlinux:base

RUN pacman -Sy --noconfirm && \
    pacman -S --noconfirm --needed \
        base-devel \
        cmake \
        ninja \
        gcc \
        pkgconf \
        qt6-base \
        qt6-tools \
        qt6-webengine \
        qscintilla-qt6 && \
    pacman -Scc --noconfirm

WORKDIR /build
COPY . /build

RUN cmake -GNinja -DCMAKE_BUILD_TYPE=Release -B build . && \
    ninja -C build && \
    echo "BUILD SUCCESS"

ENTRYPOINT ["bash"]
