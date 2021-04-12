FROM ubuntu:18.04
ARG PROJECT_INSTALL_DEPS
COPY ./ci/gitlab-ci/ubuntu/18.04/install_dependencies.sh /install_dependencies.sh
COPY ./ci/gitlab-ci/commons/install_dependencies.sh /ci/gitlab-ci/commons/install_dependencies.sh
COPY $PROJECT_INSTALL_DEPS /project_install_dependencies.sh
RUN ./install_dependencies.sh
