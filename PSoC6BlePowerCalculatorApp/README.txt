The Central is configured with
  -Prefered TX RX octets = 251
  -Prefered PHY 2 MBPS
It connects to a specific BD address and expects the peripheral to have a battery level service. It discovers all of the server's
GATT DB and then enables notifications on the Battery level charecteristic.

The peripheral can be configured for
  -Non Connectable ADV
  -Connectable ADV
  -IDLE connected state
  -Connected state with continuous notifications 
    -of size 23 bytes or 244 bytes
    -with PHY 1MBPS or 2MBPS
  
