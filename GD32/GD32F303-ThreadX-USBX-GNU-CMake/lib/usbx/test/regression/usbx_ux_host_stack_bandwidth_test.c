/* This test is designed to test the ux_host_stack_bandwidth_....  */

#include <stdio.h>
#include "tx_api.h"
#include "ux_api.h"
#include "ux_system.h"
#include "ux_utility.h"

#include "ux_host_stack.h"
#include "ux_device_stack.h"

#include "ux_device_class_cdc_acm.h"
#include "ux_host_class_cdc_acm.h"

#include "ux_host_class_dpump.h"
#include "ux_device_class_dpump.h"

#include "ux_host_class_hid.h"
#include "ux_device_class_hid.h"

#include "ux_host_class_storage.h"
#include "ux_device_class_storage.h"

#include "ux_test_dcd_sim_slave.h"
#include "ux_test_hcd_sim_host.h"
#include "ux_test_utility_sim.h"


/* Define USBX test constants.  */

#define UX_TEST_STACK_SIZE      4096
#define UX_TEST_BUFFER_SIZE     2048
#define UX_TEST_RUN             1
#define UX_TEST_MEMORY_SIZE     (64*1024)

#define     LSB(x) ( (x) & 0x00ff)
#define     MSB(x) (((x) & 0xff00) >> 8)

/* Configuration descriptor 9 bytes */
#define CFG_DESC(wTotalLength, bNumInterfaces, bConfigurationValue)\
    /* Configuration 1 descriptor 9 bytes */\
    0x09, 0x02, LSB(wTotalLength), MSB(wTotalLength),\
    (bNumInterfaces), (bConfigurationValue), 0x00,\
    0x40, 0x00,
#define CFG_DESC_LEN 9

/* DPUMP interface descriptors. */
#define DPUMP_IFC_DESC(ifc, alt, nb_ep) \
    /* Interface descriptor */\
    0x09, 0x04, (ifc), (alt), (nb_ep), 0x99, 0x99, 0x99, 0x00,

#define DPUMP_IFC_EP_DESC(epaddr, eptype, epsize) \
    /* Endpoint descriptor */\
    0x07, 0x05, (epaddr), (eptype), LSB(epsize), MSB(epsize), 0x01,

#define DPUMP_IFC_DESC_ALL_LEN(nb_ep) (9 + (nb_ep) * 7)

#define CFG_DESC_ALL_LEN (CFG_DESC_LEN + \
    DPUMP_IFC_DESC_ALL_LEN(0) + \
    DPUMP_IFC_DESC_ALL_LEN(1) + \
    DPUMP_IFC_DESC_ALL_LEN(2) + \
    DPUMP_IFC_DESC_ALL_LEN(3) + \
    DPUMP_IFC_DESC_ALL_LEN(4)   \
    )

#define CFG_DESC_ALL_LS \
    CFG_DESC(CFG_DESC_ALL_LEN, 5, 1)\
    DPUMP_IFC_DESC(0, 0, 0)\
    DPUMP_IFC_DESC(0, 1, 1)\
    DPUMP_IFC_EP_DESC(0x81, 3, 8)\
    DPUMP_IFC_DESC(0, 2, 2)\
    DPUMP_IFC_EP_DESC(0x81, 3, 8)\
    DPUMP_IFC_EP_DESC(0x02, 3, 8)\
    DPUMP_IFC_DESC(0, 3, 3)\
    DPUMP_IFC_EP_DESC(0x81, 3, 8)\
    DPUMP_IFC_EP_DESC(0x02, 3, 8)\
    DPUMP_IFC_EP_DESC(0x83, 3, 8)\
    DPUMP_IFC_DESC(0, 4, 6)\
    DPUMP_IFC_EP_DESC(0x81, 3, 8)\
    DPUMP_IFC_EP_DESC(0x02, 3, 8)\
    DPUMP_IFC_EP_DESC(0x83, 3, 8)\
    DPUMP_IFC_EP_DESC(0x04, 3, 8)\
    DPUMP_IFC_EP_DESC(0x85, 3, 8)\
    DPUMP_IFC_EP_DESC(0x06, 3, 8)

#define CFG_DESC_ALL_FS \
    CFG_DESC(CFG_DESC_ALL_LEN, 5, 1)\
    DPUMP_IFC_DESC(0, 0, 0)\
    DPUMP_IFC_DESC(0, 1, 1)\
    DPUMP_IFC_EP_DESC(0x81, 3, 64)\
    DPUMP_IFC_DESC(0, 2, 2)\
    DPUMP_IFC_EP_DESC(0x81, 3, 64)\
    DPUMP_IFC_EP_DESC(0x02, 3, 64)\
    DPUMP_IFC_DESC(0, 3, 3)\
    DPUMP_IFC_EP_DESC(0x81, 3, 64)\
    DPUMP_IFC_EP_DESC(0x02, 3, 64)\
    DPUMP_IFC_EP_DESC(0x83, 3, 64)\
    DPUMP_IFC_DESC(0, 4, 4)\
    DPUMP_IFC_EP_DESC(0x81, 3, 64)\
    DPUMP_IFC_EP_DESC(0x02, 3, 64)\
    DPUMP_IFC_EP_DESC(0x83, 3, 64)\
    DPUMP_IFC_EP_DESC(0x04, 3, 64)

#define CFG_DESC_ALL_HS \
    CFG_DESC(CFG_DESC_ALL_LEN, 5, 1)\
    DPUMP_IFC_DESC(0, 0, 0)\
    DPUMP_IFC_DESC(0, 1, 1)\
    DPUMP_IFC_EP_DESC(0x81, 3, 1024)\
    DPUMP_IFC_DESC(0, 2, 2)\
    DPUMP_IFC_EP_DESC(0x81, 3, 1024)\
    DPUMP_IFC_EP_DESC(0x02, 3, 1024)\
    DPUMP_IFC_DESC(0, 3, 3)\
    DPUMP_IFC_EP_DESC(0x81, 3, 1024)\
    DPUMP_IFC_EP_DESC(0x02, 3, 1024)\
    DPUMP_IFC_EP_DESC(0x83, 3, 1024)\
    DPUMP_IFC_DESC(0, 4, 4)\
    DPUMP_IFC_EP_DESC(0x81, 3, 1024)\
    DPUMP_IFC_EP_DESC(0x02, 3, 1024)\
    DPUMP_IFC_EP_DESC(0x83, 3, 1024)\
    DPUMP_IFC_EP_DESC(0x04, 3, 1024)

/* Define the counters used in the test application...  */

static ULONG                           thread_0_counter;
static ULONG                           thread_1_counter;
static ULONG                           error_counter;

static UCHAR                           error_callback_ignore = UX_FALSE;
static ULONG                           error_callback_counter;

static UCHAR                           buffer[UX_TEST_BUFFER_SIZE];

/* Define USBX test global variables.  */

static UX_HOST_CLASS                   *class_driver;
static UX_HOST_CLASS_DPUMP             *dpump;
static UX_SLAVE_CLASS_DPUMP            *dpump_slave = UX_NULL;

static UCHAR device_framework_low_speed[] = {

    /* Device descriptor 18 bytes */
    0x12, 0x01, 0x10, 0x01, 0x00, 0x00, 0x00, 0x08,
    0xec, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01,

    CFG_DESC_ALL_LS
};
#define DEVICE_FRAMEWORK_LENGTH_LOW_SPEED sizeof(device_framework_low_speed)

static UCHAR device_framework_full_speed[] = {

    /* Device descriptor 18 bytes */
    0x12, 0x01, 0x10, 0x01, 0x00, 0x00, 0x00, 0x08,
    0xec, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01,

    CFG_DESC_ALL_FS
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

    CFG_DESC_ALL_HS
};
#define DEVICE_FRAMEWORK_LENGTH_HIGH_SPEED sizeof(device_framework_high_speed)

/* String Device Framework :
    Byte 0 and 1 : Word containing the language ID : 0x0904 for US
    Byte 2       : Byte containing the index of the descriptor
    Byte 3       : Byte containing the length of the descriptor string
*/

static UCHAR string_framework[] = {

    /* Manufacturer string descriptor : Index 1 */
    0x09, 0x04, 0x01, 0x0c,
    0x45, 0x78, 0x70, 0x72,0x65, 0x73, 0x20, 0x4c,
    0x6f, 0x67, 0x69, 0x63,

    /* Product string descriptor : Index 2 */
    0x09, 0x04, 0x02, 0x0c,
    0x44, 0x61, 0x74, 0x61, 0x50, 0x75, 0x6d, 0x70,
    0x44, 0x65, 0x6d, 0x6f,

    /* Serial Number string descriptor : Index 3 */
    0x09, 0x04, 0x03, 0x04,
    0x30, 0x30, 0x30, 0x31
};
#define STRING_FRAMEWORK_LENGTH sizeof(string_framework)

    /* Multiple languages are supported on the device, to add
       a language besides English, the unicode language code must
       be appended to the language_id_framework array and the length
       adjusted accordingly. */
static UCHAR language_id_framework[] = {

/* English. */
    0x09, 0x04
};
#define LANGUAGE_ID_FRAMEWORK_LENGTH sizeof(language_id_framework)

/* Define prototypes for external Host Controller's (HCDs), classes and clients.  */

static VOID                ux_test_instance_activate(VOID  *dpump_instance);
static VOID                ux_test_instance_deactivate(VOID *dpump_instance);

UINT                       _ux_host_class_dpump_entry(UX_HOST_CLASS_COMMAND *command);
UINT                       ux_hcd_sim_initialize(UX_HCD *hcd);
UINT                       _ux_host_class_dpump_write(UX_HOST_CLASS_DPUMP *dpump, UCHAR * data_pointer,
                                    ULONG requested_length, ULONG *actual_length);
UINT                       _ux_host_class_dpump_read (UX_HOST_CLASS_DPUMP *dpump, UCHAR *data_pointer,
                                    ULONG requested_length, ULONG *actual_length);

static TX_THREAD           ux_test_thread_simulation_0;
static TX_THREAD           ux_test_thread_simulation_1;
static void                ux_test_thread_simulation_0_entry(ULONG);
static void                ux_test_thread_simulation_1_entry(ULONG);


/* Define the ISR dispatch.  */

extern VOID    (*test_isr_dispatch)(void);


/* Prototype for test control return.  */

void  test_control_return(UINT status);


/* Define the ISR dispatch routine.  */

static void    test_isr(void)
{

    /* For further expansion of interrupt-level testing.  */
}


static VOID error_callback(UINT system_level, UINT system_context, UINT error_code)
{

    error_callback_counter ++;

    if (!error_callback_ignore)
    {
        {
            /* Failed test.  */
            printf("Error #%d, system_level: %d, system_context: %d, error_code: 0x%x\n", __LINE__, system_level, system_context, error_code);
            test_control_return(1);
        }
    }
}

static UINT test_ux_device_class_dpump_entry(UX_SLAVE_CLASS_COMMAND *command)
{
    switch(command->ux_slave_class_command_request)
    {
        case UX_SLAVE_CLASS_COMMAND_INITIALIZE:
        case UX_SLAVE_CLASS_COMMAND_QUERY:
        case UX_SLAVE_CLASS_COMMAND_CHANGE:
            return UX_SUCCESS;

        default:
            return UX_NO_CLASS_MATCH;
    }
}

static UINT test_ux_host_class_dpump_entry(UX_HOST_CLASS_COMMAND *command)
{
    switch (command -> ux_host_class_command_request)
    {
        case UX_HOST_CLASS_COMMAND_QUERY:
        case UX_HOST_CLASS_COMMAND_ACTIVATE:
            return UX_SUCCESS;

        default:
            return UX_NO_CLASS_MATCH;
    }
}

/* Define what the initial system looks like.  */

#ifdef CTEST
void test_application_define(void *first_unused_memory)
#else
void    usbx_ux_host_stack_bandwidth_test_application_define(void *first_unused_memory)
#endif
{

UINT status;
CHAR                            *stack_pointer;
CHAR                            *memory_pointer;
UX_SLAVE_CLASS_DPUMP_PARAMETER  parameter;

    /* Initialize the free memory pointer.  */
    stack_pointer = (CHAR *) first_unused_memory;
    memory_pointer = stack_pointer + (UX_TEST_STACK_SIZE * 2);

    /* Initialize USBX Memory.  */
    status =  ux_system_initialize(memory_pointer, UX_TEST_MEMORY_SIZE, UX_NULL, 0);

    /* Check for error.  */
    if (status != UX_SUCCESS)
    {

        printf("Running ux_host_stack_bandwidth_... Test............................ ERROR #1\n");
        test_control_return(1);
    }

    /* Register the error callback. */
    _ux_utility_error_callback_register(error_callback);

    /* The code below is required for installing the host portion of USBX.  */
    status =  ux_host_stack_initialize(UX_NULL);

    /* Check for error.  */
    if (status != UX_SUCCESS)
    {

        printf("Running ux_host_stack_bandwidth_... Test............................ ERROR #2\n");
        test_control_return(1);
    }

    /* Register all the host class drivers for this USBX implementation.  */
    status =  ux_host_stack_class_register(_ux_system_host_class_dpump_name, test_ux_host_class_dpump_entry);

    /* Check for error.  */
    if (status != UX_SUCCESS)
    {

        printf("Running ux_host_stack_bandwidth_... Test............................ ERROR #3\n");
        test_control_return(1);
    }

    /* The code below is required for installing the device portion of USBX */
    status =  ux_device_stack_initialize(device_framework_high_speed, DEVICE_FRAMEWORK_LENGTH_HIGH_SPEED,
                                       device_framework_full_speed, DEVICE_FRAMEWORK_LENGTH_FULL_SPEED,
                                       string_framework, STRING_FRAMEWORK_LENGTH,
                                       language_id_framework, LANGUAGE_ID_FRAMEWORK_LENGTH, UX_NULL);

    /* Check for error.  */
    if (status != UX_SUCCESS)
    {

        printf("Running ux_host_stack_bandwidth_... Test............................ ERROR #5\n");
        test_control_return(1);
    }

    /* Set the parameters for callback when insertion/extraction of a Data Pump device.  */
    parameter.ux_slave_class_dpump_instance_activate   =  UX_NULL;
    parameter.ux_slave_class_dpump_instance_deactivate =  UX_NULL;

    /* Initialize the device dpump class. The class is connected with interface 0 */
    status =  ux_device_stack_class_register(_ux_system_slave_class_dpump_name, test_ux_device_class_dpump_entry,
                                              1, 0, &parameter);

    /* Check for error.  */
    if (status != UX_SUCCESS)
    {

        printf("Running ux_host_stack_bandwidth_... Test............................ ERROR #6\n");
        test_control_return(1);
    }

    /* Initialize the simulated device controller.  */
    status =  _ux_test_dcd_sim_slave_initialize();

    /* Check for error.  */
    if (status != UX_SUCCESS)
    {

        printf("Running ux_host_stack_bandwidth_... Test............................ ERROR #7\n");
        test_control_return(1);
    }

    /* Register all the USB host controllers available in this system */
    status =  ux_host_stack_hcd_register(_ux_system_host_hcd_simulator_name, _ux_test_hcd_sim_host_initialize,0,0);

    /* Check for error.  */
    if (status != UX_SUCCESS)
    {

        printf("Running ux_host_stack_bandwidth_... Test............................ ERROR #4\n");
        test_control_return(1);
    }

    /* Create the main host simulation thread.  */
    status =  tx_thread_create(&ux_test_thread_simulation_0, "test host simulation", ux_test_thread_simulation_0_entry, 0,
            stack_pointer, UX_TEST_STACK_SIZE,
            20, 20, 1, TX_AUTO_START);

    /* Check for error.  */
    if (status != TX_SUCCESS)
    {

        printf("Running ux_host_stack_bandwidth_... Test............................ ERROR #8\n");
        test_control_return(1);
    }
}


static void  ux_test_thread_simulation_0_entry(ULONG arg)
{

UINT                     status;
INT                      i;
INT                      test;
UX_HCD                  *hcd;
UX_DEVICE               *device;
UX_CONFIGURATION        *configuration;
UX_INTERFACE            *interface;
UX_INTERFACE            *interfaces[8];
UX_DEVICE                hub[3];
struct _test_def {
    UCHAR     speed;
    UCHAR     *framework;
    ULONG     framework_length;
    ULONG     hcd_version;
    ULONG     available_bandwidth;
    UX_DEVICE *parent;
    UCHAR     parent_speed;
    ULONG     tt_mask;
    ULONG     tt_bandwidth;
    UINT      status0;
    UINT      status1_invert;
} tests[] = {
    {UX_LOW_SPEED_DEVICE , device_framework_low_speed , DEVICE_FRAMEWORK_LENGTH_LOW_SPEED , 0x110,   48*8*5, UX_NULL},
    {UX_LOW_SPEED_DEVICE , device_framework_low_speed , DEVICE_FRAMEWORK_LENGTH_LOW_SPEED , 0x200,   48    , UX_NULL},
    {UX_FULL_SPEED_DEVICE, device_framework_full_speed, DEVICE_FRAMEWORK_LENGTH_FULL_SPEED, 0x110,  128*5  , UX_NULL},
    {UX_FULL_SPEED_DEVICE, device_framework_full_speed, DEVICE_FRAMEWORK_LENGTH_FULL_SPEED, 0x200,  128    , UX_NULL},
    {UX_HIGH_SPEED_DEVICE, device_framework_high_speed, DEVICE_FRAMEWORK_LENGTH_HIGH_SPEED, 0x110, 2048    , UX_NULL},
    {UX_HIGH_SPEED_DEVICE, device_framework_high_speed, DEVICE_FRAMEWORK_LENGTH_HIGH_SPEED, 0x200, 2048    , UX_NULL},

    {UX_FULL_SPEED_DEVICE, device_framework_full_speed, DEVICE_FRAMEWORK_LENGTH_FULL_SPEED, 0x200, 6000    , &hub[2], UX_HIGH_SPEED_DEVICE, 0x0, 128, UX_NO_BANDWIDTH_AVAILABLE},
    {UX_FULL_SPEED_DEVICE, device_framework_full_speed, DEVICE_FRAMEWORK_LENGTH_FULL_SPEED, 0x200, 6000    , &hub[2], UX_HIGH_SPEED_DEVICE, 0x3, 128},

    {UX_FULL_SPEED_DEVICE, device_framework_full_speed, DEVICE_FRAMEWORK_LENGTH_FULL_SPEED, 0x200, 6000    , &hub[2], UX_FULL_SPEED_DEVICE, 0x3, 128, UX_SUCCESS, UX_TRUE},
};

#if UX_MAX_DEVICES > 1

    /* Inform user.  */
    printf("Running ux_host_stack_bandwidth_... Test............................ ");

    hcd = &_ux_system_host -> ux_system_host_hcd_array[0];

    /* Build a hub list for tests: hub0 (hs) <- hub1 (fs) <- hub2 (fs). */
    ux_utility_memory_set(hub, 0, sizeof(hub));
    hub[0].ux_device_speed = UX_HIGH_SPEED_DEVICE;
    hub[0].ux_device_parent = UX_NULL;
    hub[0].ux_device_port_location = 0;
    hub[0].ux_device_hub_tt[0].ux_hub_tt_max_bandwidth = UX_TT_BANDWIDTH;
    hub[0].ux_device_hub_tt[0].ux_hub_tt_port_mapping = 0;
    hub[0].ux_device_hub_tt[1].ux_hub_tt_max_bandwidth = UX_TT_BANDWIDTH;
    hub[0].ux_device_hub_tt[1].ux_hub_tt_port_mapping = UX_TT_MASK;
    hub[1].ux_device_speed = UX_FULL_SPEED_DEVICE;
    hub[1].ux_device_parent = &hub[0];
    hub[1].ux_device_port_location = 2;
    hub[2].ux_device_speed = UX_FULL_SPEED_DEVICE;
    hub[2].ux_device_parent = &hub[1];
    hub[2].ux_device_port_location = 3;

    ux_test_hcd_sim_host_connect(UX_FULL_SPEED_DEVICE);

    error_callback_ignore = UX_TRUE;

    for (test = 0; test < sizeof(tests)/sizeof(struct _test_def); test ++)
    {

        /* Disconnect. */
        ux_test_dcd_sim_slave_disconnect();
        ux_test_hcd_sim_host_disconnect();

        /* Connect. */
        hcd -> ux_hcd_version = tests[test].hcd_version;
        hcd -> ux_hcd_available_bandwidth = tests[test].available_bandwidth;
        ux_test_dcd_sim_slave_connect_framework(tests[test].framework, tests[test].framework_length);
        ux_test_hcd_sim_host_connect(tests[test].speed);
        tx_thread_sleep(200);

        status = ux_host_stack_device_get(0, &device);
        if (status != UX_SUCCESS)
        {

            printf("ERROR #%d.%d: device_get fail\n", __LINE__, test);
            test_control_return(1);
        }

        status = ux_host_stack_device_configuration_get(device, 0, &configuration);
        if (status != UX_SUCCESS)
        {

            printf("ERROR #%d.%d: cfg_get fail\n", __LINE__, test);
            test_control_return(1);
        }

        interface = configuration->ux_configuration_first_interface;
        for (i = 0; i < 8; i ++)
        {

            if (interface == UX_NULL)
                break;

            interfaces[i] = interface;
            interface = interface->ux_interface_next_interface;
        }

        /* Set configure. */
        status = ux_host_stack_device_configuration_select(configuration);
        if (status != UX_SUCCESS)
        {

            printf("ERROR #%d.%d: cfg_set fail\n", __LINE__, test);
            test_control_return(1);
        }

        /* Modify parent. */
        if (tests[test].parent)
        {
            device->ux_device_parent = tests[test].parent;
            hub[0].ux_device_speed = tests[test].parent_speed;
            hub[0].ux_device_hub_tt[1].ux_hub_tt_port_mapping = tests[test].tt_mask;
            hub[0].ux_device_hub_tt[1].ux_hub_tt_max_bandwidth = tests[test].tt_bandwidth;
        }

        /* Switch to interface 0.1, bandwith OK */
        status = ux_host_stack_interface_setting_select(interfaces[1]);
        if (status != tests[test].status0)
        {

            printf("ERROR #%d.%d: ifc_set status not expected: %x<>%x\n", __LINE__, test, tests[test].status0, status);
            error_counter ++;
        }

        /* Switch to interface 0.4, bandwidth FAIL */
        error_callback_counter = 0;
        status = ux_host_stack_interface_setting_select(interfaces[4]);
        if (tests[test].status1_invert)
        {
            if (status != UX_SUCCESS)
            {

                printf("ERROR #%d.%d: Expect ifc_set pass\n", __LINE__, test);
                error_counter ++;
            }
        }
        else
        {
            if (status == UX_SUCCESS)
            {

                printf("ERROR #%d.%d: Expect ifc_set fail\n", __LINE__, test);
                error_counter ++;
            }
            if (error_callback_counter == 0)
            {

                printf("ERROR #%d.%d: Expect error\n", __LINE__, test);
                error_counter ++;
            }
        }

        /* Restore parent. */
        if (tests[test].parent)
        {

            device->ux_device_parent = UX_NULL;
            hub[0].ux_device_speed = UX_HIGH_SPEED_DEVICE;
            hub[0].ux_device_hub_tt[1].ux_hub_tt_port_mapping = UX_TT_MASK;
            hub[0].ux_device_hub_tt[1].ux_hub_tt_max_bandwidth = UX_TT_BANDWIDTH;
        }
    }
    error_callback_ignore = UX_FALSE;

#else

    /* Inform user.  */
    printf("Running ux_host_stack_bandwidth_... Skip for max 1 device .......... ");
#endif

    /* Sleep for a tick to make sure everything is complete.  */
    tx_thread_sleep(1);

    /* Check for errors from other threads.  */
    if (error_counter)
    {

        /* Test error.  */
        printf("ERROR #14\n");
        test_control_return(1);
    }
    else
    {

        /* Successful test.  */
        printf("SUCCESS!\n");
        test_control_return(0);
    }
}
