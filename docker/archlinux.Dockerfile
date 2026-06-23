FROM archlinux:latest

RUN pacman -Syyu --noconfirm \
    && pacman -S --noconfirm ninja clang cmake openssh vim git tmux zellij \
    && mkdir -p /var/run/sshd \
    && sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/g' /etc/ssh/sshd_config \
    && ssh-keygen -A \
    && echo "root:123456" | chpasswd
