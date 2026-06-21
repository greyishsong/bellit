FROM archlinux:latest

RUN pacman -Syyu --noconfirm \
    && pacman -S --noconfirm ninja clang cmake openssh vim git tmux \
    && mkdir -p /var/run/sshd \
    && sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/g' /etc/ssh/sshd_config \
    && echo "root:123456" | chpasswd
