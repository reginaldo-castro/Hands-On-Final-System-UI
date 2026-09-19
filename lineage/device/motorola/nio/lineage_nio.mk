#
# Copyright (C) 2022 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

# Inherit from nio device
$(call inherit-product, device/motorola/nio/device.mk)

# Device identifier. This must come after all inclusions.
PRODUCT_NAME := lineage_nio
PRODUCT_DEVICE := nio
PRODUCT_MODEL := Moto G100 (Handson OS)
PRODUCT_BRAND := Handson
LINEAGE_VERSION_APPEND := -HandsonEdition

PRODUCT_SYSTEM_NAME := nio_retcn

# Build info
BUILD_FINGERPRINT := "motorola/nio_retcn/nio:12/S1RN32.55-16-13/fce21a-b9f4e9:user/release-keys"
PRODUCT_BUILD_PROP_OVERRIDES += \
    PRIVATE_BUILD_DESC="nio_retcn-user 12 S1RN32.55-16-13 fce21a-b9f4e9 release-keys" \
    TARGET_PRODUCT=$(PRODUCT_SYSTEM_NAME)

# Handson custom charger
PRODUCT_PACKAGES += handson_charger
PRODUCT_PACKAGES += init.handson_charger.rc