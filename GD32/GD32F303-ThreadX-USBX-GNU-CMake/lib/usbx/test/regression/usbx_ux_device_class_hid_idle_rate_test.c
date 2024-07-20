/* This file tests the
 * _ux_device_class_hid_control_request
 * _ux_device_class_hid_interrupt_thread
 */

#include "usbx_test_common_hid.h"

#include "ux_test_dcd_sim_slave.h"
#include "ux_test_hcd_sim_host.h"
#include "ux_test_utility_sim.h"

#include "ux_host_class_hid_mouse.h"
#include "ux_host_class_hid_keyboard.h"

#define DUMMY_USBX_MEMORY_SIZE          (64*1024)

static UCHAR                            dummy_usbx_memory[DUMMY_USBX_MEMORY_SIZE];

static UX_SLAVE_CLASS_HID               *slave_hid = UX_NULL;
static UX_SLAVE_CLASS_HID_EVENT         slave_hid_event;

#define HOST_BUFFER_LENGTH              32

static UCHAR                            host_buffer[HOST_BUFFER_LENGTH];
static ULONG                            host_buffer_length;

static UCHAR hid_report_descriptor[] = {

    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,                    // USAGE (Keyboard)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
    0x95, 0x05,                    //   REPORT_COUNT (5)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x05, 0x08,                    //   USAGE_PAGE (LEDs)
    0x19, 0x01,                    //   USAGE_MINIMUM (Num Lock)
    0x29, 0x05,                    //   USAGE_MAXIMUM (Kana)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x03,                    //   REPORT_SIZE (3)
    0x91, 0x03,                    //   OUTPUT (Cnst,Var,Abs)
    0x95, 0x06,                    //   REPORT_COUNT (6)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
    0xc0                           // END_COLLECTION
};
#define HID_REPORT_LENGTH sizeof(hid_report_descriptor)/sizeof(hid_report_descriptor[0])

/* Configuration descriptor 9 bytes */
#define CFG_DESC(wTotalLength, bNumInterfaces, bConfigurationValue)\
    /* Configuration 1 descriptor 9 bytes */\
    0x09, 0x02, LSB(wTotalLength), MSB(wTotalLength),\
    (bNumInterfaces), (bConfigurationValue), 0x00,\
    0x40, 0x00,
#define CFG_DESC_LEN 9

/* HID Mouse interface descriptors 9+9+7=25 bytes */
#define HID_MOUSE_IFC_DESC_ALL(ifc, interrupt_epa)     \
    /* Interface descriptor */\
    0x09, 0x04, (ifc), 0x00, 0x01, 0x03, 0x00, 0x00, 0x00,\
    /* HID descriptor */\
    0x09, 0x21, 0x10, 0x01, 0x21, 0x01, 0x22, LSB(HID_REPORT_LENGTH),\
    MSB(HID_REPORT_LENGTH),\
    /* Endpoint descriptor (Interrupt) */\
    0x07, 0x05, (interrupt_epa), 0x03, 0x08, 0x00, 0x08,
#define HID_MOUSE_IFC_DESC_ALL_LEN 25

/* HID Mouse interface descriptors 9+9+7+7=32 bytes */
#define HID_TEST_IFC_DESC_ALL(ifc, epa0, epa0_type, epa1, epa1_type)     \
    /* Interface descriptor */\
    0x09, 0x04, (ifc), 0x00, 0x02, 0x03, 0x00, 0x00, 0x00,\
    /* HID descriptor */\
    0x09, 0x21, 0x10, 0x01, 0x21, 0x01, 0x22, LSB(HID_REPORT_LENGTH),\
    MSB(HID_REPORT_LENGTH),\
    /* Endpoint descriptor */\
    0x07, 0x05, (epa0), (epa0_type), 0x08, 0x00, 0x08,\
    /* Endpoint descriptor */\
    0x07, 0x05, (epa1), (epa1_type), 0x08, 0x00, 0x08,
#define HID_TEST_IFC_DESC_ALL_LEN 32

static UCHAR device_framework_full_speed[] = {

    /* Device descriptor */
    0x12, 0x01, 0x10, 0x01, 0x00, 0x00, 0x00, 0x08,
    0x81, 0x0A, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01,

    CFG_DESC(CFG_DESC_LEN+2*HID_TEST_IFC_DESC_ALL_LEN, 2, 1)
    /* Interrupt IN @ 1st */
    HID_TEST_IFC_DESC_ALL(0, 0x81, 3, 0x02, 3)
    /* HID interface but no interrupt IN */
    HID_TEST_IFC_DESC_ALL(1, 0x84, 2, 0x03, 3)
};
#define DEVICE_FRAMEWORK_LENGTH_FULL_SPEED sizeof(device_framework_full_speed)


static UCHAR device_framework_high_speed[] = {

    /* Device descriptor */
    0x12, 0x01, 0x00, 0x02, 0x00, 0x00, 0x00, 0x40,
    0x0a, 0x07, 0x25, 0x40, 0x01, 0x00, 0x01, 0x02,
    0x03, 0x01,

    /* Device qualifier descriptor */
    0x0a, 0x06, 0x00, 0x02, 0x00, 0x00, 0x00, 0x40,
    0x01, 0x00,

    CFG_DESC(CFG_DESC_LEN+HID_MOUSE_IFC_DESC_ALL_LEN+HID_TEST_IFC_DESC_ALL_LEN, 2, 1)
    /* Good HID interface. */
    HID_MOUSE_IFC_DESC_ALL(0, 0x81)
    /* Interrupt IN @ 2nd. */
    HID_TEST_IFC_DESC_ALL(1, 0x03, 3, 0x84, 3)
};
#define DEVICE_FRAMEWORK_LENGTH_HIGH_SPEED sizeof(device_framework_high_speed)


    /* String Device Framework :
     Byte 0 and 1 : Word containing the language ID : 0x0904 for US
     Byte 2       : Byte containing the index of the descriptor
     Byte 3       : Byte containing the length of the descriptor string
    */

#define STRING_FRAMEWORK_LENGTH 40
static UCHAR string_framework[] = {

    /* Manufacturer string descriptor : Index 1 */
        0x09, 0x04, 0x01, 0x0c,
        0x45, 0x78, 0x70, 0x72,0x65, 0x73, 0x20, 0x4c,
        0x6f, 0x67, 0x69, 0x63,

    /* Product string descriptor : Index 2 */
        0x09, 0x04, 0x02, 0x0c,
        0x55, 0x53, 0x42, 0x20, 0x4b, 0x65, 0x79, 0x62,
        0x6f, 0x61, 0x72, 0x64,

    /* Serial Number string descriptor : Index 3 */
        0x09, 0x04, 0x03, 0x04,
        0x30, 0x30, 0x30, 0x31
    };


    /* Multiple languages are supported on the device, to add
       a language besides english, the unicode language code must
       be appended to the language_id_framework array and the length
       adjusted accordingly. */
#define LANGUAGE_ID_FRAMEWORK_LENGTH 2
static UCHAR language_id_framework[] = {

    /* English. */
        0x09, 0x04
    };


UINT  _ux_hcd_sim_host_entry(UX_HCD *hcd, UINT function, VOID *parameter);


static UINT ux_system_host_change_function(ULONG a, UX_HOST_CLASS *b, VOID *c)
{
    return 0;
}

static VOID instance_activate_callback(VOID *parameter)
{

    slave_hid = (UX_SLAVE_CLASS_HID *)parameter;
}

static VOID error_callback(UINT system_level, UINT system_context, UINT error_code)
{

    // printf("Error on line %d: 0x%x, 0x%x, 0x%x\n", __LINE__, system_level, system_context, error_code);
}

/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    usbx_ux_device_class_hid_idle_rate_test_application_define(void *first_unused_memory)
#endif
{

UINT status;
CHAR *                          stack_pointer;
CHAR *                          memory_pointer;


    /* Inform user.  */
    printf("Running ux_device_class_hid_ Idle Rate tests ....................... ");
    stepinfo("\n");

    /* Initialize the free memory pointer */
    stack_pointer = (CHAR *) usbx_memory;
    memory_pointer = stack_pointer + (UX_DEMO_STACK_SIZE * 2);

    /* Initialize USBX. Memory */
    status = ux_system_initialize(memory_pointer, UX_DEMO_MEMORY_SIZE, UX_NULL,0);

    /* Check for error.  */
    if (status != UX_SUCCESS)
    {

        printf("Error on line %d, error code: 0x%x\n", __LINE__, status);
        test_control_return(1);
    }

    /* Register the error callback. */
    _ux_utility_error_callback_register(error_callback);

    /* The code below is required for installing the host portion of USBX */
    status =  ux_host_stack_initialize(ux_system_host_change_function);
    if (status != UX_SUCCESS)
    {

        printf("Error on line %d, error code: 0x%x\n", __LINE__, status);
        test_control_return(1);
    }

    status =  ux_host_stack_class_register(_ux_system_host_class_hid_name, ux_host_class_hid_entry);
    if (status != UX_SUCCESS)
    {

        printf("Error on line %d, error code: 0x%x\n", __LINE__, status);
        test_control_return(1);
    }
#if 0
    /* Register the HID client(s).  */
    status =  ux_host_class_hid_client_register(_ux_system_host_class_hid_client_keyboard_name, ux_host_class_hid_keyboard_entry);
    if (status != UX_SUCCESS)
    {

        printf("Error on line %d, error code: %d\n", __LINE__, status);
        test_control_return(1);
    }
#endif
    /* The code below is required for installing the device portion of USBX. No call back for
       device status change in this example. */
    status =  ux_device_stack_initialize(device_framework_high_speed, DEVICE_FRAMEWORK_LENGTH_HIGH_SPEED,
                                       device_framework_full_speed, DEVICE_FRAMEWORK_LENGTH_FULL_SPEED,
                                       string_framework, STRING_FRAMEWORK_LENGTH,
                                       language_id_framework, LANGUAGE_ID_FRAMEWORK_LENGTH,UX_NULL);
    if(status!=UX_SUCCESS)
    {

        printf("Error on line %d, error code: 0x%x\n", __LINE__, status);
        test_control_return(1);
    }

    /* Initialize the hid class parameters.  */
    hid_parameter.ux_device_class_hid_parameter_report_address = hid_report_descriptor;
    hid_parameter.ux_device_class_hid_parameter_report_length  = HID_REPORT_LENGTH;
    hid_parameter.ux_device_class_hid_parameter_callback       = demo_thread_hid_callback;
    hid_parameter.ux_slave_class_hid_instance_activate         = instance_activate_callback;

    /* Initialize the device hid class. The class is connected with interface 2 */
    status  = ux_device_stack_class_register(_ux_system_slave_class_hid_name, ux_device_class_hid_entry,
                                             1,0, (VOID *)&hid_parameter);
    if(status!=UX_SUCCESS)
    {

        printf("Error on line %d, error code: 0x%x\n", __LINE__, status);
        test_control_return(1);
    }

    /* Initialize the simulated device controller.  */
    status =  _ux_dcd_sim_slave_initialize();

    /* Check for error.  */
    if (status != UX_SUCCESS)
    {

        printf("Error on line %d, error code: 0x%x\n", __LINE__, status);
        test_control_return(1);
    }

    /* Register all the USB host controllers available in this system */
    status =  ux_host_stack_hcd_register(_ux_system_host_hcd_simulator_name, ux_hcd_sim_host_initialize,0,0);

    /* Check for error.  */
    if (status != UX_SUCCESS)
    {

        printf("Error on line %d, error code: 0x%x\n", __LINE__, status);
        test_control_return(1);
    }

    /* Create the main host simulation thread.  */
    status =  tx_thread_create(&tx_demo_thread_host_simulation, "tx demo host simulation", tx_demo_thread_host_simulation_entry, 0,
            stack_pointer, UX_DEMO_STACK_SIZE,
            20, 20, 1, TX_AUTO_START);

    /* Check for error.  */
    if (status != TX_SUCCESS)
    {

        printf("Error on line %d, error code: 0x%x\n", __LINE__, status);
        test_control_return(1);
    }
}

static ULONG _report_count = 0;
static void _run_idle_rate_test(INT test, UCHAR idle_rate, UX_SLAVE_CLASS_HID_EVENT *event)
{

UINT   status;
USHORT actual_rate;
ULONG  count;
ULONG  expected_count, actual_count;


    if (event)
    {
        stepinfo(">>>>>>>>>> #%d - event set\n", test);

        status = ux_host_class_hid_idle_set(hid, 0, 0);
        if (status != UX_SUCCESS)
        {

            printf("#%d, Error on line %d, error code: %x\n", test, __LINE__, status);
            test_control_return(1);
        }

        _ux_utility_thread_suspend(&slave_hid -> ux_slave_class_hid_interface -> ux_slave_interface_class -> ux_slave_class_thread);
        _ux_device_class_hid_event_set(slave_hid, event);
        _ux_device_class_hid_event_set(slave_hid, event);
        _ux_device_class_hid_event_set(slave_hid, event);
        _ux_utility_thread_resume(&slave_hid -> ux_slave_class_hid_interface -> ux_slave_interface_class -> ux_slave_class_thread);
    }

    stepinfo(">>>>>>>>>> #%d - Set_Idle : %d\n", test, idle_rate);

    status = ux_host_class_hid_idle_set(hid, idle_rate, 0);
    if (status != UX_SUCCESS)
    {

        printf("#%d, Error on line %d, error code: %x\n", test, __LINE__, status);
        test_control_return(1);
    }

    stepinfo(">>>>>>>>>> #%d - Get_Idle : %d\n", test, idle_rate);

    status = ux_host_class_hid_idle_get(hid, &actual_rate, 0);
    if (status != UX_SUCCESS)
    {

        printf("#%d, Error on line %d, error code: %x\n", test, __LINE__, status);
        test_control_return(1);
    }
    if (actual_rate != idle_rate)
    {

        printf("#%d, Error on line %d, idle_rate: %d\n", test, __LINE__, actual_rate);
        test_control_return(1);
    }

    if (idle_rate > 0)
    {
        stepinfo(">>>>>>>>>> #%d - Wait %d for idle rate change\n", test, 1000);
        count = _report_count;
        _ux_utility_delay_ms(1000);
        actual_count = _report_count - count;

        /* Expected count 1000 / (idle_rate * 4).  */
        expected_count = 1000 / (idle_rate * 4);
        stepinfo(">>>>>>>>>> #%d, report count expect %ld vs %ld\n", test, expected_count, actual_count);
        if ((1000 / UX_PERIODIC_RATE) * 5 > (idle_rate * 4))
        {
            stepinfo(">>>>>>>>>> #%d, count check skipped\n", test);
        }
        else
        {

            /* ~30% area.  */
            if (expected_count * 13 / 10 < actual_count ||
                expected_count *  7 / 10 > actual_count)
            {
                printf("#%d, Error on line %d, report count not expected: %ld\n", test, __LINE__, actual_count);
                test_control_return(1);
            }
        }

        if (event)
        {
            stepinfo(">>>>>>>>>> #%d - check event data\n", test);
            if (host_buffer_length != event->ux_device_class_hid_event_length)
            {
                printf("#%d, Error on line %d, report length not expected: %ld\n", test, __LINE__, host_buffer_length);
                test_control_return(1);
            }
            if (_ux_utility_memory_compare(host_buffer, event->ux_device_class_hid_event_buffer, host_buffer_length))
            {
                printf("#%d, Error on line %d, report data not expected\n", test, __LINE__);
                printf("event:\n");
                for (count = 0; count < host_buffer_length; count ++)
                    printf(" %2x", event->ux_device_class_hid_event_buffer[count]);
                printf("\n");
                printf("host buffer:\n");
                for (count = 0; count < host_buffer_length; count ++)
                    printf(" %2x", host_buffer[count]);
                printf("\n");
                test_control_return(1);
            }
        }
    }
    else
    {
        stepinfo(">>>>>>>>>> #%d - Wait %d for idle rate to take action\n", test, 1000);
        _ux_utility_delay_ms(1000);

        stepinfo(">>>>>>>>>> #%d - Wait %d to confirm there is no event\n", test, 1000);
        count = _report_count;
        _ux_utility_delay_ms(1000);
        actual_count = _report_count - count;
        if (count != _report_count)
        {
            printf("#%d, Error on line %d, report count changed %ld\n", test, __LINE__, actual_count);
            test_control_return(1);
        }
    }
}

static VOID  _hid_report_callback(UX_HOST_CLASS_HID_REPORT_CALLBACK *callback)
{
    ULONG length;
    _report_count ++;
    length = callback->ux_host_class_hid_report_callback_actual_length;
    if (length > HOST_BUFFER_LENGTH)
        length = HOST_BUFFER_LENGTH;
    _ux_utility_memory_copy(host_buffer,
            callback->ux_host_class_hid_report_callback_buffer,
            length);
    host_buffer_length = length;
}

static void  tx_demo_thread_host_simulation_entry(ULONG arg)
{

UINT                                status;
UX_DEVICE                           *device;
UX_ENDPOINT                         *control_endpoint;
UX_TRANSFER                         *transfer_request;
UX_HOST_CLASS_HID_REPORT_GET_ID     report_id;
UX_HOST_CLASS_HID_REPORT_CALLBACK   call_back;
USHORT                              idle_rate;

    stepinfo(">>>>>>>>>> Thread start\n");

    _ux_utility_delay_ms(500);

    /* Get device instance. */
    status = ux_host_stack_device_get(0, &device);

    if (status != UX_SUCCESS)
    {
        printf("ERROR #%d: get_device fail, 0x%x\n", __LINE__, status);
        test_control_return(1);
    }

    /* Get control endpoint. */
    control_endpoint = &device->ux_device_control_endpoint;
    transfer_request = &control_endpoint->ux_endpoint_transfer_request;

    stepinfo(">>>>>>>>>> Get HID class instance\n");

    /* Find the HID class */
    status = demo_class_hid_get();
    if (status != UX_SUCCESS)
    {

        printf("Error on line %d, error code: %d\n", __LINE__, status);
        test_control_return(1);
    }
    if (slave_hid == UX_NULL)
    {
        printf("Error on line %d, HID slave instance error\n", __LINE__);
        test_control_return(1);
    }

    /* Get the report ID for the keyboard. The keyboard is a INPUT report.
    This should be 0 but in case. */
    report_id.ux_host_class_hid_report_get_report = UX_NULL;
    report_id.ux_host_class_hid_report_get_type = UX_HOST_CLASS_HID_REPORT_TYPE_INPUT;
    status =  _ux_host_class_hid_report_id_get(hid, &report_id);
    if (status != UX_SUCCESS)
    {
        printf("ERROR #%d: get_report_id fail, 0x%x\n", __LINE__, status);
        test_control_return(1);
    }

    /* Initialize the report callback.  */
    call_back.ux_host_class_hid_report_callback_id =         report_id.ux_host_class_hid_report_get_id;
    call_back.ux_host_class_hid_report_callback_function =   _hid_report_callback;
    call_back.ux_host_class_hid_report_callback_buffer =     UX_NULL;
    call_back.ux_host_class_hid_report_callback_flags =      UX_HOST_CLASS_HID_REPORT_RAW;
    call_back.ux_host_class_hid_report_callback_length =     0;

    /* Register the report call back when data comes it on this report.  */
    status =  _ux_host_class_hid_report_callback_register(hid, &call_back);
    if (status != UX_SUCCESS)
    {
        printf("ERROR #%d: report_callback_register fail, 0x%x\n", __LINE__, status);
        test_control_return(1);
    }

    /* Start periodic report (to poll reports).  */
    status = _ux_host_class_hid_periodic_report_start(hid);
    if (status != UX_SUCCESS)
    {
        printf("ERROR #%d: report_start fail, 0x%x\n", __LINE__, status);
        test_control_return(1);
    }

    stepinfo(">>>>>>>>>> Get_Idle : 0\n");

    status = ux_host_class_hid_idle_get(hid, &idle_rate, 0);
    if (status != UX_SUCCESS)
    {

        printf("Error on line %d, error code: %x\n", __LINE__, status);
        test_control_return(1);
    }
    if (idle_rate != 0)
    {

        printf("Error on line %d, idle_rate: %d\n", __LINE__, idle_rate);
        test_control_return(1);
    }

    _run_idle_rate_test(__LINE__, 4/4,   0);

    slave_hid_event.ux_device_class_hid_event_report_id = 0;
    slave_hid_event.ux_device_class_hid_event_report_type = 0;

    slave_hid_event.ux_device_class_hid_event_length = 8;
    slave_hid_event.ux_device_class_hid_event_buffer[0] = 1;
    slave_hid_event.ux_device_class_hid_event_buffer[1] = 1;
    slave_hid_event.ux_device_class_hid_event_buffer[2] = 1;
    slave_hid_event.ux_device_class_hid_event_buffer[3] = 1;
    slave_hid_event.ux_device_class_hid_event_buffer[4] = 1;
    slave_hid_event.ux_device_class_hid_event_buffer[5] = 1;
    slave_hid_event.ux_device_class_hid_event_buffer[6] = 1;
    slave_hid_event.ux_device_class_hid_event_buffer[7] = 1;
    _run_idle_rate_test(__LINE__, 200/4, &slave_hid_event);

    slave_hid_event.ux_device_class_hid_event_length = 6;
    slave_hid_event.ux_device_class_hid_event_buffer[0] = 2;
    slave_hid_event.ux_device_class_hid_event_buffer[1] = 4;
    slave_hid_event.ux_device_class_hid_event_buffer[2] = 1;
    slave_hid_event.ux_device_class_hid_event_buffer[3] = 3;
    slave_hid_event.ux_device_class_hid_event_buffer[4] = 3;
    slave_hid_event.ux_device_class_hid_event_buffer[5] = 3;
    slave_hid_event.ux_device_class_hid_event_buffer[6] = 0;
    slave_hid_event.ux_device_class_hid_event_buffer[7] = 0;
    _run_idle_rate_test(__LINE__, 100/4, &slave_hid_event);

    _run_idle_rate_test(__LINE__, 0,     0);

    _ux_utility_delay_ms(500);

    stepinfo(">>>>>>>>>> Test done\n");

    /* Now disconnect the device.  */
    _ux_device_stack_disconnect();

    /* And deinitialize the class.  */
    status =  ux_device_stack_class_unregister(_ux_system_slave_class_hid_name, ux_device_class_hid_entry);

    /* Deinitialize the device side of usbx.  */
    _ux_device_stack_uninitialize();

    /* And finally the usbx system resources.  */
    _ux_system_uninitialize();

    /* Successful test.  */
    printf("SUCCESS!\n");
    test_control_return(0);
}

static UINT    demo_thread_hid_callback(UX_SLAVE_CLASS_HID *class, UX_SLAVE_CLASS_HID_EVENT *event)
{
    return(UX_SUCCESS);
}
