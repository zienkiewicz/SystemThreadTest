# SystemThreadTest
## exercise content
> After reading some online forums, you notice some people suggesting that `PsCreateSystemThread` will create a thread in the context of the calling process. In other words, they are suggesting that if you call `PsCreateSystemThread` in an IOCTL handler, the new thread will be in the context of the requesting user-mode application. Assess the validity of this statement by writing a driver that calls `PsCreateSystemThread` in the IOCTL handler. Next, experiment with a non-NULL `ProcessHandle` and determine if the context differs

## Test
```
sc create SystemThreadTest type=kernel binPath=<path to the .sys>
sc start SystemThreadTest
```
