# from https://github.com/esphome/esphome/blob/dev/docker/Dockerfile

FROM ubuntu:23.04 AS base-docker

RUN \
    apt-get update \
    && apt-get install -y --no-install-recommends \
    python3 \
    python3-pip \
    python3-setuptools \
    python3-pil \
    python3-cryptography \
    python3-venv \
    python3-dev \
    iputils-ping \
    git \
    curl \
    wget \
    less \
    vim \
    build-essential \
    clang-format-13 \
    clang-tidy \
    patch \
    software-properties-common \
    nano \
    openssh-client \
    && rm -rf \
    /tmp/* \
    /var/{cache,log}/* \
    /var/lib/apt/lists/*

ENV \
    # Fix click python3 lang warning https://click.palletsprojects.com/en/7.x/python3/
    LANG=C.UTF-8 LC_ALL=C.UTF-8 \
    # Store globally installed pio libs in /piolibs
    PLATFORMIO_GLOBALLIB_DIR=/opt/piolibs \
    PLATFORMIO_CORE_DIR=/workspaces/piotech/.temp/platformio

# create a local user, and remmove root password
RUN \
    sed -i 's/root:\*:/root::/' /etc/shadow \
    && useradd -m -U piouser -s /bin/bash \
    && chown piouser:piouser /opt

# COPY esphome /esphome

# RUN \
#     chown -R piouser:piouser /esphome

# create a python venv for piouser
USER piouser
RUN \
    python3 -m venv /opt/venv

# Enable venv
ENV PATH="/opt/venv/bin:$PATH"

RUN \
    # Ubuntu python3-pip is missing wheel
    pip3 install --no-cache-dir  wheel



# First install requirements to leverage caching when requirements don't change
#COPY docker/requirements.txt docker/platformio_install_deps.py platformio.ini /
#RUN \
#    pip3 install --no-cache-dir -r /requirements.txt \
#    && platformio settings set enable_telemetry No \
#    && platformio settings set check_platformio_interval 1000000 \
#    && mkdir /opt/piolibs
#    && /platformio_install_deps.py /platformio.ini --libraries --tools --platforms

# RUN pip3 install -v --no-cache-dir --no-use-pep517 -e /esphome

RUN \
    mkdir /opt/piolibs \
    && chown piouser:piouser /opt/piolibs


# Settings for dashboard
ENV USERNAME="" PASSWORD=""

# Expose the dashboard to Docker
EXPOSE 6052


USER piouser

#COPY /docker/entrypoint.sh /entrypoint.sh

# The directory the user should mount their configuration files to
# VOLUME ["/piotech"]
WORKDIR /piotech



# Set entrypoint to esphome (via a script) so that the user doesn't have to type 'esphome'
# in every docker command twice
# ENTRYPOINT ["/entrypoint.sh"]

# When no arguments given, start the dashboard in the workdir
CMD ["esphome","dashboard", "config/"]

# disabled, no needs daemon
#ADD https://github.com/just-containers/s6-overlay/releases/download/v${S6_OVERLAY_VERSION}/s6-overlay-noarch.tar.xz /tmp
#RUN tar -C / -Jxpf /tmp/s6-overlay-noarch.tar.xz
#ADD https://github.com/just-containers/s6-overlay/releases/download/v${S6_OVERLAY_VERSION}/s6-overlay-x86_64.tar.xz /tmp
#RUN tar -C / -Jxpf /tmp/s6-overlay-x86_64.tar.xz
#ENTRYPOINT ["/init"]




