UID=$(shell id -u)
USER=$(shell id -u -n)
GID=$(shell id -g)
GROUP=$(shell id -g -n)
PWD=$(shell pwd)
container:
	docker build \
	--build-arg uid=$(UID) \
	--build-arg user=$(USER) \
	--build-arg gid=$(GID) \
	--build-arg group=$(GROUP) \
	. --tag devcontainer

# run: container
# 	docker run -i -t --rm --volume $(PWD)/src/:/home/$(USER)/src --user=$(USER) devcontainer bash