FROM archlinux:base-devel

# Pre-select providers: jack=jack2, ttf-font=noto-fonts; avoid all interactive prompts
RUN echo -e "1\n2\n" | pacman -Sy --noconfirm \
        qt6-base qt6-tools qt6-webengine qscintilla-qt6 \
        cmake ninja gcc pkgconf && \
    rm -rf /var/cache/pacman/pkg

WORKDIR /build
COPY . /build

RUN cmake -GNinja -DCMAKE_BUILD_TYPE=Release -B build . && \
    ninja -C build && \
    echo "BUILD SUCCESS"

ENTRYPOINT ["bash"]
