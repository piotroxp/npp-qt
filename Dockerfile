FROM archlinux:base-devel

# Pre-select provider packages to avoid interactive prompts during install
# jack: 1=jack2, ttf-font: 2=noto-fonts
RUN echo -e "1\n2\n" | pacman -Sy --noconfirm \
        qt6-base qt6-tools qt6-webengine qscintilla-qt6 \
        cmake ninja gcc pkgconf && \
    rm -rf /var/cache/pacman/pkg

WORKDIR /build
COPY . /build

RUN cmake -GNinja -DCMAKE_BUILD_TYPE=Release -B build .
RUN ninja -C build

ENTRYPOINT ["bash"]
