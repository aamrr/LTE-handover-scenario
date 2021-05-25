
//Added for flow monitor
#include "ns3/flow-monitor.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/lte-module.h"
#include "ns3/applications-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/config-store-module.h"
#include "ns3/netanim-module.h"
using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("LenaX2HandoverMeasures");

void
 NotifyConnectionEstablishedUe (std::string context,
                                uint64_t imsi,
                                uint16_t cellid,
                                uint16_t rnti)
 {
   std::cout << context
             << " UE IMSI " << imsi
             << ": connected to CellId " << cellid
             << " with RNTI " << rnti
             << std::endl;
 }
 
 void
 NotifyHandoverStartUe (std::string context,
                        uint64_t imsi,
                        uint16_t cellid,
                        uint16_t rnti,
                        uint16_t targetCellId)
 {
   std::cout << context
             << " UE IMSI " << imsi
             << ": previously connected to CellId " << cellid
             << " with RNTI " << rnti
             << ", doing handover to CellId " << targetCellId
             << std::endl;
 }
 
 void
 NotifyHandoverEndOkUe (std::string context,
                        uint64_t imsi,
                        uint16_t cellid,
                        uint16_t rnti)
 {
   std::cout << context
             << " UE IMSI " << imsi
             << ": successful handover to CellId " << cellid
             << " with RNTI " << rnti
             << std::endl;
 }
 
 void
 NotifyConnectionEstablishedEnb (std::string context,
                                 uint64_t imsi,
                                 uint16_t cellid,
                                 uint16_t rnti)
 {
   std::cout << context
             << " eNB CellId " << cellid
             << ": successful connection of UE with IMSI " << imsi
             << " RNTI " << rnti
             << std::endl;
 }
 
 void
 NotifyHandoverStartEnb (std::string context,
                         uint64_t imsi,
                         uint16_t cellid,
                         uint16_t rnti,
                         uint16_t targetCellId)
 {
   std::cout << context
             << " eNB CellId " << cellid
             << ": start handover of UE with IMSI " << imsi
             << " RNTI " << rnti
             << " to CellId " << targetCellId
             << std::endl;
 }
 
 void
 NotifyHandoverEndOkEnb (std::string context,
                         uint64_t imsi,
                         uint16_t cellid,
                         uint16_t rnti)
 {
   std::cout << context
             << " eNB CellId " << cellid
             << ": completed handover of UE with IMSI " << imsi
             << " RNTI " << rnti
             << std::endl;
 }
 

int main (int argc, char *argv[])
{
  CommandLine cmd;

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();
  //This will instantiate some common objects (e.g., the Channel object) and provide the methods to add eNBs and UEs and configure them.

  Config::SetDefault ("ns3::UdpClient::Interval", TimeValue (MilliSeconds (1000)));
  Config::SetDefault ("ns3::UdpClient::MaxPackets", UintegerValue (100000));
  Config::SetDefault ("ns3::LteHelper::UseIdealRrc", BooleanValue (false));

  // defining handover algorithm
  Ptr<PointToPointEpcHelper> epcHelper = CreateObject<PointToPointEpcHelper> ();
   lteHelper->SetEpcHelper (epcHelper);
   lteHelper->SetSchedulerType ("ns3::RrFfMacScheduler");
 
   lteHelper->SetHandoverAlgorithmType ("ns3::A2A4RsrqHandoverAlgorithm");
   lteHelper->SetHandoverAlgorithmAttribute ("ServingCellThreshold",
                                             UintegerValue (30));
   lteHelper->SetHandoverAlgorithmAttribute ("NeighbourCellOffset",
                                             UintegerValue (1));
 
  // internet 
  Ptr<Node> pgw = epcHelper->GetPgwNode ();
 
  // Create a single RemoteHost
  NodeContainer remoteHostContainer;
  remoteHostContainer.Create (1);
  Ptr<Node> remoteHost = remoteHostContainer.Get (0);
  InternetStackHelper internet;
  internet.Install (remoteHostContainer);

  // Create the Internet
  PointToPointHelper p2ph;
  p2ph.SetDeviceAttribute ("DataRate", DataRateValue (DataRate ("100Gb/s")));
  p2ph.SetDeviceAttribute ("Mtu", UintegerValue (1500));
  p2ph.SetChannelAttribute ("Delay", TimeValue (Seconds (0.010)));
  NetDeviceContainer internetDevices = p2ph.Install (pgw, remoteHost);
  Ipv4AddressHelper ipv4h;
  ipv4h.SetBase ("1.0.0.0", "255.0.0.0");
  Ipv4InterfaceContainer internetIpIfaces = ipv4h.Assign (internetDevices);
  Ipv4Address remoteHostAddr = internetIpIfaces.GetAddress (1);


  // Routing of the Internet Host (towards the LTE network)
  Ipv4StaticRoutingHelper ipv4RoutingHelper;
  Ptr<Ipv4StaticRouting> remoteHostStaticRouting = ipv4RoutingHelper.GetStaticRouting (remoteHost->GetObject<Ipv4> ());
  // interface 0 is localhost, 1 is the p2p device
  remoteHostStaticRouting->AddNetworkRouteTo (Ipv4Address ("7.0.0.0"), Ipv4Mask ("255.0.0.0"), 1);


  //Create Node objects for the eNB(s) and the UEs:

  NodeContainer enbNodes;
  enbNodes.Create (2);
  NodeContainer ueNodes;
  ueNodes.Create (6);


  // Install Mobility Model in eNB
   Ptr<ListPositionAllocator> enbPositionAlloc = CreateObject<ListPositionAllocator> ();
   
   Vector enbPosition (300 , 200, 0);
   enbPositionAlloc->Add (enbPosition);

   Vector enbPosition1 (600 , 200, 0);
   enbPositionAlloc->Add (enbPosition1);

   MobilityHelper enbMobility;
   enbMobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
   enbMobility.SetPositionAllocator (enbPositionAlloc);
   enbMobility.Install (enbNodes);
 
   // Install Mobility Model in UE
   MobilityHelper ueMobility;
   ueMobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");
   ueMobility.Install (ueNodes);
   for (uint16_t i = 0; i < 3; i++)
    {
      ueNodes.Get (i)->GetObject<MobilityModel> ()->SetPosition (Vector (0, 10 * (i), 0));
      ueNodes.Get (i)->GetObject<ConstantVelocityMobilityModel> ()->SetVelocity (Vector (20, 10+i*3, 0));
    }
    for (uint16_t i = 3; i < 6; i++)
    {
      ueNodes.Get (i)->GetObject<MobilityModel> ()->SetPosition (Vector (800, 10 * (i), 0));
      ueNodes.Get (i)->GetObject<ConstantVelocityMobilityModel> ()->SetVelocity (Vector (-20, 10 + (i-3)*3, 0));
    }

  //Install an LTE protocol stack on the eNB(s):

  NetDeviceContainer enbDevs;
  enbDevs = lteHelper->InstallEnbDevice (enbNodes);

  //Install an LTE protocol stack on the UEs:

  NetDeviceContainer ueDevs;
  ueDevs = lteHelper->InstallUeDevice (ueNodes);

  // Install the IP stack on the UEs
   internet.Install (ueNodes);
   Ipv4InterfaceContainer ueIpIfaces;
   ueIpIfaces = epcHelper->AssignUeIpv4Address (NetDeviceContainer (ueDevs));

  //Attach the UEs to an eNB. This will configure each UE according to the eNB configuration, and create an RRC connection between them:

   for (uint16_t i = 0; i < 3; i++)
     {
       lteHelper->Attach (ueDevs.Get (i), enbDevs.Get (0));
     }
    for (uint16_t i = 3; i < 6; i++)
     {
       lteHelper->Attach (ueDevs.Get (i), enbDevs.Get (1));
     }


  NS_LOG_LOGIC ("setting up applications");
  uint16_t dlPort = 10000;
  uint16_t ulPort = 20000;
  Ptr<UniformRandomVariable> startTimeSeconds = CreateObject<UniformRandomVariable> ();
   startTimeSeconds->SetAttribute ("Min", DoubleValue (0));
   startTimeSeconds->SetAttribute ("Max", DoubleValue (0.010));
//Activate a data radio bearer between each UE and the eNB it is attached to:
  for (uint32_t u = 0; u < 6; ++u)
     {
       Ptr<Node> ue = ueNodes.Get (u);
       // Set the default gateway for the UE
       Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting (ue->GetObject<Ipv4> ());
       ueStaticRouting->SetDefaultRoute (epcHelper->GetUeDefaultGatewayAddress (), 1);
 
       for (uint32_t b = 0; b < 1; ++b)
         {
           ++dlPort;
           ++ulPort;
 
           ApplicationContainer clientApps;
           ApplicationContainer serverApps;
 
           NS_LOG_LOGIC ("installing UDP DL app for UE " << u);
           UdpClientHelper dlClientHelper (ueIpIfaces.GetAddress (u), dlPort);
           clientApps.Add (dlClientHelper.Install (remoteHost));
           PacketSinkHelper dlPacketSinkHelper ("ns3::UdpSocketFactory",
                                                InetSocketAddress (Ipv4Address::GetAny (), dlPort));
           serverApps.Add (dlPacketSinkHelper.Install (ue));
 
           NS_LOG_LOGIC ("installing UDP UL app for UE " << u);
           UdpClientHelper ulClientHelper (remoteHostAddr, ulPort);
           clientApps.Add (ulClientHelper.Install (ue));
           PacketSinkHelper ulPacketSinkHelper ("ns3::UdpSocketFactory",
                                                InetSocketAddress (Ipv4Address::GetAny (), ulPort));
           serverApps.Add (ulPacketSinkHelper.Install (remoteHost));
 
           Ptr<EpcTft> tft = Create<EpcTft> ();
           EpcTft::PacketFilter dlpf;
           dlpf.localPortStart = dlPort;
           dlpf.localPortEnd = dlPort;
           tft->Add (dlpf);
           EpcTft::PacketFilter ulpf;
           ulpf.remotePortStart = ulPort;
           ulpf.remotePortEnd = ulPort;
           tft->Add (ulpf);
           EpsBearer bearer (EpsBearer::NGBR_VIDEO_TCP_DEFAULT);
           lteHelper->ActivateDedicatedEpsBearer (ueDevs.Get (u), bearer, tft);
 
           Time startTime = Seconds (startTimeSeconds->GetValue ());
           serverApps.Start (startTime);
           clientApps.Start (startTime);
 
         } // end for b
     }

     // Add X2 interface for the handover activation
    lteHelper->AddX2Interface (enbNodes);



  // connect custom trace sinks for RRC connection establishment and handover notification
   Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/ConnectionEstablished",
                    MakeCallback (&NotifyConnectionEstablishedEnb));
   Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/ConnectionEstablished",
                    MakeCallback (&NotifyConnectionEstablishedUe));
   Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverStart",
                    MakeCallback (&NotifyHandoverStartEnb));
   Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/HandoverStart",
                    MakeCallback (&NotifyHandoverStartUe));
   Config::Connect ("/NodeList/*/DeviceList/*/LteEnbRrc/HandoverEndOk",
                    MakeCallback (&NotifyHandoverEndOkEnb));
   Config::Connect ("/NodeList/*/DeviceList/*/LteUeRrc/HandoverEndOk",
                    MakeCallback (&NotifyHandoverEndOkUe));

  // Flow monitor
  Ptr<FlowMonitor> flowMonitor;
  FlowMonitorHelper flowHelper;
  flowMonitor = flowHelper.InstallAll();

  //Set the stop time
  Simulator::Stop (Seconds (45));

  ///This is needed otherwise the simulation will last forever, because (among others) the start-of-subframe event is scheduled repeatedly, and the ns-3 simulator scheduler will hence never run out of events.
    lteHelper->EnablePhyTraces ();
    lteHelper->EnableMacTraces ();
    lteHelper->EnableRlcTraces ();


  //Run the simulation:
    AnimationInterface anim ("lte_handover_anim.xml");
    anim.SetMobilityPollInterval(Seconds(1));
    anim.SetMaxPktsPerTraceFile (1000000000);
  // anim.EnablePacketMetadata(true);
  Simulator::Run ();
  flowMonitor->SerializeToXmlFile("lte_log.xml", true, true);
  Simulator::Destroy ();
  return 0;
}