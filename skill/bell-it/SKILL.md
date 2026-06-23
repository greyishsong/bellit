---
name: bell-it
description: Send a desktop notification to the user.
license: GPL-2.0
metadata:
  author: greyishsong
---

A desktop notification provides a more significant signal to the user than a message in the conversation, suitable for notifying after a long running task has succeeded or failed.

Send a normal notification like this:
```shell
bellit -t "Your title" -m "Your message"
```

Or send a warning after failure:
```shell
bellit -t "Your title" -w "Your warning message"
```

Determine what to notify according to these principles:
- The title (passed through `-t` option) should summarize the task itself in **several words**.
- The message (passed through `-m` option) or warning message (passed through `-w` message) should summarize the result (or reason) in **one sentence**.
- Both of them MUST be brief and informative, because a desktop notification cannot display many characters.

`bellit` can also be used to send a notification after an external command has returned. Run `bellit --help` for detailed usage.
