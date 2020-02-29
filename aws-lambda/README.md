# How to install on Amazon Lambda?
Base on [project issue #35](https://github.com/bbc/audiowaveform/issues/35)

Its possible to run the audiowaveform in AWS-lambda environment, thanks for:
- [@chrisn](https://github.com/chrisn);
- [@ffxsam](https://github.com/ffxsam);
- [@ajbarber](https://github.com/ajbarber)

- [@TrevorHinesley](https://github.com/TrevorHinesley) found a error in [this comment](https://github.com/bbc/audiowaveform/issues/35#issuecomment-589107734)

What docker image can I use? amazonlinux:latest or amazonlinux:1 ?
- [see this aws documentarion](https://docs.aws.amazon.com/lambda/latest/dg/lambda-runtimes.html)

To build the 'audiowaveform' to your aws-lambda using docker its simple, you only need a docker installed and running in your system:
- [read this wiki for more info](https://wiki.archlinux.org/index.php/Docker)

# How?
```bash
git clone https://github.com/bbc/audiowaveform.git
cd audiowaveform/aws-lambda

#the default is to use amazonlinux:latest image:
./run.sh

#if you need to use amazonlinux:1 image, run as follow:
./run.sh amazonlinux-1
```

after the end of the script execution, in case of success you can see the follow message in you terminal:
> Binary available in bin directory

have fun with audiowaveform in your aws-lambda!

I case of something go wrong, feel free to edit [Dockerfile](./amazonlinux-latest/Dockerfile) or [scripts/build.sh](./scripts/build.sh). In this case you can use aditional options of `run.sh` script:
```bash
# In case of change the scripts/build.sh you need only to rerun it in your container without update the docker image:
./run.sh

# If you change the Dockerfile you need to update the docker image and rerun the script in a new container
./run.sh amazonlinux-latest update

# If you aready runned the docker image / docker container, and only need to copy the audiowaveform again from your container, run:
./run.sh amazonlinux-latest install

# You can also delete all the docker things
./run.sh amazonlinux-latest clean
```
