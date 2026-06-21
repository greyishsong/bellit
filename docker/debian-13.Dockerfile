FROM debian:13

RUN apt-get update \
    && apt-get install -y ninja-build clang clang-tools openssh-server vim git iproute2 tmux wget ca-certificates build-essential debhelper dpkg-dev fakeroot \
    && wget -c https://github.com/Kitware/CMake/releases/download/v4.2.7/cmake-4.2.7-linux-x86_64.sh -O /root/cmake.sh \
    && chmod a+x /root/cmake.sh \
    && mkdir -p /opt/cmake \
    && /root/cmake.sh --prefix=/opt/cmake --exclude-subdir --skip-license \
    && echo "\nexport PATH=\"/opt/cmake/bin:\$PATH\"" >> /root/.bashrc \
    && mkdir -p /var/run/sshd \
    && sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/g' /etc/ssh/sshd_config \
    && echo "root:123456" | chpasswd
