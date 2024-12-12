# Release Notes

## Version 0.1.2 - 2024-12-12

###  Feature
- **Communication task: 
        ***Send data over standard CAN.
        ***Add timestamp to 3dImage snapshot.
        ***Add 2 bytes for identify CAN frame (LSB and MSB) and pattern to identify EOF. 
- **Scripts: Added master node simulator with Image3D reception and send global time
###  Fix
- **Global time sync task: fix issues with CAN frame index 

## Version 0.1.1 - 2024-12-07

###  Fix
- **CI/CD pipeline: Fix generate tag release job when merged is done

## Version 0.1.0 - 2024-12-01

###  New features
- **CI/CD pipeline: Add generate tag release job
- **Update HW file with the specific HW for the LIDAR node

## Version 0.0.2 - 2024-11-29

###  New features
- **CI/CD integration: Add version-check.yml and release-notes-check.yml

## Version 0.0.1 - 2024-11-27

###  New features
- **DataSerializer: Serialize the Image3D snapshot and split it in CAN messages
- **Fixes: Share image3D snapshot between tasks using OSAL component

