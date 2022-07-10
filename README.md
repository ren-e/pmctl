# macOS pmctl
> :warning: **Only Intel systems supported at this moment**

This application allows you to manipulate private power management settings
 from the command line interface.

### Requirements
  - macOS >= 12.0
  - Apple Xcode Command Line Tools

### Models supported
  - Apple MacBook Pro >= 2016
  - Apple Macbook Air >= 2018

### Compile
```
make
```

### Synopsis
```
pmctl [-b socmax] [-b charge] [-d discharge] [-r]
```

### Description
The pmcctl utility manipulates private power management settings, such as
 limiting the battery SoC. You're also able to discharge the battery, while
 being plugged in. This program only supports Apple Macbook systems, and could
 result in undefined behavior when using different systems.

The options are as follows:
```
	[-b socmax]	Configure the battery SoC maximum value.
	[-c charge]	Allows to charge the battery (1: enable, 0: disable).
	[-d discharge]	Discharge the battery until %.
	[-r]		Read current or applied power management settings.
```


### Caveats
The parameters `-b` and `-c` require root, other parameters can be run as the
 regular user.
When using the `-d` parameter the application will wait until the discharge
 threshold has reached. Press CTRL+C to cancel the discharge.
