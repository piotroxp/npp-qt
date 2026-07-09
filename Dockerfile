FROM archlinux:base-devel

RUN pacman -Sy --noconfirm qt6-base qt6-tools qscintilla-qt6 cmake ninja gcc pkgconf && \
    pacman -Scc --noconfirm

WORKDIR /build
COPY . /build

RUN cmake -GNinja -DCMAKE_BUILD_TYPE=Release -B build .
RUN ninja -C build

ENTRYPOINT ["bash"]
