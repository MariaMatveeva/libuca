
#include <stdlib.h>
#include <string.h>
#include <pcilib.h>
#include "uca.h"
#include "uca-cam.h"
#include "uca-grabber.h"

#define set_void(p, type, value) { *((type *) p) = value; }
#define GET_HANDLE(cam) ((pcilib_t *) cam->user)

static void uca_ipe_handle_error(const char *format, ...)
{
    /* Do nothing, we just check errno. */
}

static uint32_t uca_ipe_set_property(struct uca_camera_priv *cam, enum uca_property_ids property, void *data)
{
    pcilib_t *handle = GET_HANDLE(cam);
    pcilib_register_value_t value = *((pcilib_register_value_t *) data);

    switch (property) {
        case UCA_PROP_EXPOSURE:
            pcilib_write_register(handle, NULL, "exp_time", value);
            break;
            
        default:
            return UCA_ERR_CAMERA | UCA_ERR_PROP | UCA_ERR_INVALID;
    }
    return UCA_NO_ERROR;
}

static uint32_t uca_ipe_get_property(struct uca_camera_priv *cam, enum uca_property_ids property, void *data, size_t num)
{
    pcilib_t *handle = GET_HANDLE(cam);
    pcilib_register_value_t value = 0;

    switch (property) {
        case UCA_PROP_NAME:
            strncpy((char *) data, "IPE PCIe based on CMOSIS CMV2000", num);
            break;

        case UCA_PROP_BITDEPTH:
            set_void(data, uint32_t, 16);
            break;

        case UCA_PROP_WIDTH:
            set_void(data, uint32_t, 2048);
            break;

        case UCA_PROP_HEIGHT:
            set_void(data, uint32_t, 1088);
            break;

        case UCA_PROP_EXPOSURE:
            pcilib_read_register(handle, NULL, "exp_time", &value);
            set_void(data, uint32_t, (uint32_t) value);

        case UCA_PROP_TEMPERATURE_SENSOR:
            pcilib_read_register(handle, NULL, "cmosis_temperature", &value);
            set_void(data, uint32_t, (uint32_t) value);
            break;

        case UCA_PROP_PGA_GAIN:
            pcilib_read_register(handle, NULL, "pga", &value);
            set_void(data, uint32_t, (uint32_t) value);
            break;

        case UCA_PROP_PGA_GAIN_MIN:
            set_void(data, uint32_t, 0);
            break;

        case UCA_PROP_PGA_GAIN_MAX:
            set_void(data, uint32_t, 3);
            break;

        case UCA_PROP_ADC_GAIN:
            pcilib_read_register(handle, NULL, "adc_gain", &value);
            set_void(data, uint32_t, (uint32_t) value);
            break;

        case UCA_PROP_ADC_GAIN_MIN:
            set_void(data, uint32_t, 32);
            break;

        case UCA_PROP_ADC_GAIN_MAX:
            set_void(data, uint32_t, 55);
            break;

        default:
            return UCA_ERR_CAMERA | UCA_ERR_PROP | UCA_ERR_INVALID;
    }
    return UCA_NO_ERROR;
}

static uint32_t uca_ipe_start_recording(struct uca_camera_priv *cam)
{
    pcilib_t *handle = cam->user;
    pcilib_start(handle, PCILIB_EVENT_DATA, PCILIB_EVENT_FLAGS_DEFAULT);
    return UCA_NO_ERROR;
}

static uint32_t uca_ipe_stop_recording(struct uca_camera_priv *cam)
{
    pcilib_t *handle = cam->user;
    pcilib_stop(handle, PCILIB_EVENT_FLAGS_DEFAULT);
    return UCA_NO_ERROR;
}

static uint32_t uca_ipe_grab(struct uca_camera_priv *cam, char *buffer, void *meta_data)
{
    pcilib_t *handle = cam->user;
    size_t size = cam->frame_width * cam->frame_height * sizeof(uint16_t);
    void *data = NULL;
    if (pcilib_grab(handle, PCILIB_EVENTS_ALL, &size, &data, PCILIB_TIMEOUT_INFINITE))
        return UCA_ERR_CAMERA;
    memcpy(buffer, data, size);
    free(data);
    return UCA_NO_ERROR;
}

static uint32_t uca_ipe_register_callback(struct uca_camera_priv *cam, uca_cam_grab_callback cb, void *user)
{
    if (cam->callback == NULL) {
        cam->callback = cb;
        cam->callback_user = user;
        return UCA_NO_ERROR;
    }
    return UCA_ERR_CAMERA | UCA_ERR_CALLBACK | UCA_ERR_ALREADY_REGISTERED;
}

static uint32_t uca_ipe_destroy(struct uca_camera_priv *cam)
{
    pcilib_close(GET_HANDLE(cam));
    return UCA_NO_ERROR;
}

uint32_t uca_ipe_init(struct uca_camera_priv **cam, struct uca_grabber_priv *grabber)
{
    pcilib_model_t model = PCILIB_MODEL_DETECT;
    pcilib_set_error_handler(uca_ipe_handle_error, uca_ipe_handle_error);
    pcilib_t *handle = pcilib_open("/dev/fpga0", model);
    /* XXX: This is not working because pcilib is still returning a valid
     * structure although things like "failing ioctl's" can happen. */
    if (handle == NULL)
        return UCA_ERR_CAMERA | UCA_ERR_INIT | UCA_ERR_NOT_FOUND;

    pcilib_set_error_handler(&uca_ipe_handle_error, &uca_ipe_handle_error);
    model = pcilib_get_model(handle);

    struct uca_camera_priv *uca = uca_cam_new();

    /* Camera found, set function pointers... */
    uca->destroy = &uca_ipe_destroy;
    uca->set_property = &uca_ipe_set_property;
    uca->get_property = &uca_ipe_get_property;
    uca->start_recording = &uca_ipe_start_recording;
    uca->stop_recording = &uca_ipe_stop_recording;
    uca->grab = &uca_ipe_grab;
    uca->register_callback = &uca_ipe_register_callback;

    uca->frame_width = 2048;
    uca->frame_height = 1088;
    uca->user = handle;
    *cam = uca;

    return UCA_NO_ERROR;
}
