/*
 * Copyright (c) 2018 NITK Surathkal
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Authors: Vivek Jain <jain.vivek.anand@gmail.com>
 *          Viyom Mittal <viyommittal@gmail.com>
 *          Mohit P. Tahiliani <tahiliani@nitk.edu.in>
 */

#include "tcp-bbr-v2.h"

#include "ns3/log.h"
#include "ns3/simulator.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE("TcpBbrV2");
NS_OBJECT_ENSURE_REGISTERED(TcpBbrV2);

const double TcpBbrV2::PACING_GAIN_CYCLE[] = {5.0 / 4, 3.0 / 4, 1, 1, 1, 1, 1, 1};

TypeId
TcpBbrV2::GetTypeId()
{
    static TypeId tid =
        TypeId("ns3::TcpBbrV2")
            .SetParent<TcpCongestionOps>()
            .AddConstructor<TcpBbrV2>()
            .SetGroupName("Internet")
            .AddAttribute("Stream",
                          "Random number stream (default is set to 4 to align with Linux results)",
                          UintegerValue(4),
                          MakeUintegerAccessor(&TcpBbrV2::SetStream),
                          MakeUintegerChecker<uint32_t>())
            .AddAttribute("HighGain",
                          "Value of high gain",
                          DoubleValue(2.89),
                          MakeDoubleAccessor(&TcpBbrV2::m_highGain),
                          MakeDoubleChecker<double>())
            .AddAttribute("BwWindowLength",
                          "Length of bandwidth windowed filter",
                          UintegerValue(10),
                          MakeUintegerAccessor(&TcpBbrV2::m_bandwidthWindowLength),
                          MakeUintegerChecker<uint32_t>())
            .AddAttribute("RttWindowLength",
                          "Length of RTT windowed filter",
                          TimeValue(Seconds(10)),
                          MakeTimeAccessor(&TcpBbrV2::m_minRttFilterLen),
                          MakeTimeChecker())
            .AddAttribute("ProbeRttWindowLength",
                          "Length of ProbeRTT windowed filter",
                          TimeValue(Seconds(5)),
                          MakeTimeAccessor(&TcpBbrV2::m_probeRttMinFilterLen),
                          MakeTimeChecker())
            .AddAttribute("ProbeRttCwndGain",
                          "Cwnd gain for ProbeRTT",
                          DoubleValue(0.5),
                          MakeDoubleAccessor(&TcpBbrV2::m_probeRttCwndGain),
                          MakeDoubleChecker<double>())
            .AddAttribute("ProbeRttDuration",
                          "Time to be spent in PROBE_RTT phase",
                          TimeValue(MilliSeconds(200)),
                          MakeTimeAccessor(&TcpBbrV2::m_probeRttDuration),
                          MakeTimeChecker())
            .AddAttribute("ExtraAckedRttWindowLength",
                          "Window length of extra acked window",
                          UintegerValue(5),
                          MakeUintegerAccessor(&TcpBbrV2::m_extraAckedWinRttLength),
                          MakeUintegerChecker<uint32_t>())
            .AddAttribute(
                "AckEpochAckedResetThresh",
                "Max allowed val for m_ackEpochAcked, after which sampling epoch is reset",
                UintegerValue(1 << 12),
                MakeUintegerAccessor(&TcpBbrV2::m_ackEpochAckedResetThresh),
                MakeUintegerChecker<uint32_t>())
            .AddAttribute("LossThreshold",
                          "Stop probing when loss rate exceeds this level",
                          DoubleValue(0.02),
                          MakeDoubleAccessor(&TcpBbrV2::m_lossThresh),
                          MakeDoubleChecker<double>())
            .AddAttribute("FullLossCount",
                          "Exit STARTUP when number of loss events exceeds this level",
                          UintegerValue(8),
                          MakeUintegerAccessor(&TcpBbrV2::m_fullLossCount),
                          MakeUintegerChecker<uint32_t>())
            .AddAttribute("EcnAlphaGain",
                          "Gain factor for ECN mark ratio samples",
                          DoubleValue(0.0625),
                          MakeDoubleAccessor(&TcpBbrV2::m_ecnAlphaGain),
                          MakeDoubleChecker<double>())
            .AddAttribute("EcnFactor",
                          "On ECN, cut inflight_lo to (1 - m_ecnFactor * m_ecnAlpha)",
                          DoubleValue(0.333),
                          MakeDoubleAccessor(&TcpBbrV2::m_ecnFactor),
                          MakeDoubleChecker<double>())
            .AddAttribute("EcnThreshold",
                          "Stop probing bw when CE ratio exceeds this threshold",
                          DoubleValue(0.5),
                          MakeDoubleAccessor(&TcpBbrV2::m_ecnThresh),
                          MakeDoubleChecker<double>())
            .AddAttribute("EcnMaxRtt",
                          "Max RTT (in usec) at which to use sender-side ECN logic",
                          TimeValue(MilliSeconds(5)),
                          MakeTimeAccessor(&TcpBbrV2::m_ecnMaxRtt),
                          MakeTimeChecker())
            .AddAttribute("FullEcnCount",
                          "Exit STARTUP if number of round trips with ECN mark rate above m_ecnThresh meets this count",
                          DoubleValue(0.02),
                          MakeUintegerAccessor(&TcpBbrV2::m_fullEcnCount),
                          MakeUintegerChecker<uint32_t>())
            .AddAttribute("Beta",
                          "On losses, scale down inflight and pacing rate by beta",
                          DoubleValue(0.3),
                          MakeDoubleAccessor(&TcpBbrV2::m_beta),
                          MakeDoubleChecker<double>())
            .AddAttribute("InflightHeadroom",
                          "Fraction of unutilized headroom to try to leave in path upon high loss",
                          DoubleValue(0.15),
                          MakeDoubleAccessor(&TcpBbrV2::m_inflightHeadroom),
                          MakeDoubleChecker<double>())
            .AddTraceSource("MinRtt",
                            "Estimated two-way round-trip propagation delay of the path, estimated "
                            "from the windowed minimum recent round-trip delay sample",
                            MakeTraceSourceAccessor(&TcpBbrV2::m_minRtt),
                            "ns3::TracedValueCallback::Time")
            .AddTraceSource("PacingGain",
                            "The dynamic pacing gain factor",
                            MakeTraceSourceAccessor(&TcpBbrV2::m_pacingGain),
                            "ns3::TracedValueCallback::Double")
            .AddTraceSource("CwndGain",
                            "The dynamic congestion window gain factor",
                            MakeTraceSourceAccessor(&TcpBbrV2::m_cWndGain),
                            "ns3::TracedValueCallback::Double");
    return tid;
}

TcpBbrV2::TcpBbrV2()
    : TcpCongestionOps()
{
    NS_LOG_FUNCTION(this);
    m_uv = CreateObject<UniformRandomVariable>();
}

TcpBbrV2::TcpBbrV2(const TcpBbrV2& sock)
    : TcpCongestionOps(sock),
      m_bandwidthWindowLength(sock.m_bandwidthWindowLength),
      m_pacingGain(sock.m_pacingGain),
      m_cWndGain(sock.m_cWndGain),
      m_highGain(sock.m_highGain),
      m_isPipeFilled(sock.m_isPipeFilled),
      m_minPipeCwnd(sock.m_minPipeCwnd),
      m_roundCount(sock.m_roundCount),
      m_roundStart(sock.m_roundStart),
      m_nextRoundDelivered(sock.m_nextRoundDelivered),
      m_probeRttDuration(sock.m_probeRttDuration),
      m_probeRtPropStamp(sock.m_probeRtPropStamp),
      m_probeRttDoneStamp(sock.m_probeRttDoneStamp),
      m_probeRttRoundDone(sock.m_probeRttRoundDone),
      m_probeRttCwndGain(sock.m_probeRttCwndGain),
      m_packetConservation(sock.m_packetConservation),
      m_priorCwnd(sock.m_priorCwnd),
      m_idleRestart(sock.m_idleRestart),
      m_targetCWnd(sock.m_targetCWnd),
      m_fullBandwidth(sock.m_fullBandwidth),
      m_fullBandwidthCount(sock.m_fullBandwidthCount),
      m_sendQuantum(sock.m_sendQuantum),
      m_cycleStamp(sock.m_cycleStamp),
      m_cycleIndex(sock.m_cycleIndex),
      m_minRtt(Time::Max()),
      m_minRttExpired(sock.m_minRttExpired),
      m_minRttFilterLen(sock.m_minRttFilterLen),
      m_minRttStamp(sock.m_minRttStamp),
      m_probeRttMin(Time::Max()),
      m_probeRttMinExpired(sock.m_probeRttMinExpired),
      m_probeRttMinFilterLen(sock.m_probeRttMinFilterLen),
      m_probeRttMinStamp(sock.m_probeRttMinStamp),
      m_isInitialized(sock.m_isInitialized),
      m_uv(sock.m_uv),
      m_delivered(sock.m_delivered),
      m_appLimited(sock.m_appLimited),
      m_extraAckedGain(sock.m_extraAckedGain),
      m_extraAckedWinRtt(sock.m_extraAckedWinRtt),
      m_extraAckedWinRttLength(sock.m_extraAckedWinRttLength),
      m_ackEpochAckedResetThresh(sock.m_ackEpochAckedResetThresh),
      m_extraAckedIdx(sock.m_extraAckedIdx),
      m_ackEpochTime(sock.m_ackEpochTime),
      m_ackEpochAcked(sock.m_ackEpochAcked),
      m_hasSeenRtt(sock.m_hasSeenRtt),
      m_lossThresh(sock.m_lossThresh),
      m_fullLossCount(sock.m_fullLossCount),
      m_lossInCycle(sock.m_lossInCycle),
      m_lossInRound(sock.m_lossInRound),
      m_lossRoundStart(sock.m_lossRoundStart),
      m_lossRoundDelivered(sock.m_lossRoundDelivered),
      m_lossEventsInRound(sock.m_lossEventsInRound),
      m_ecnAlphaGain(sock.m_ecnAlphaGain),
      m_ecnFactor(sock.m_ecnFactor),
      m_ecnThresh(sock.m_ecnThresh),
      m_ecnMaxRtt(sock.m_ecnMaxRtt),
      m_fullEcnCount(sock.m_fullEcnCount),
      m_startupEcnRounds(sock.m_startupEcnRounds),
      m_ecnInCycle(sock.m_ecnInCycle),
      m_ecnEligible(sock.m_ecnEligible),
      m_ecnAlpha(sock.m_ecnAlpha),
      m_ecnInRound(sock.m_ecnInRound),
      m_alphaLastDelivered(sock.m_alphaLastDelivered),
      m_alphaLastDeliveredCe(sock.m_alphaLastDeliveredCe),
      m_beta(sock.m_beta),
      m_inflightHi(sock.m_inflightHi),
      m_inflightLo(sock.m_inflightLo),
      m_inflightLatest(sock.m_inflightLatest),
      m_bwLo(sock.m_bwLo),
      m_bwLatest(sock.m_bwLatest),
      m_bwProbeUpCount(sock.m_bwProbeUpCount),
      m_bwProbeUpAcks(sock.m_bwProbeUpAcks),
      m_bwProbeUpRounds(sock.m_bwProbeUpRounds),
      m_stoppedRiskyProbe(sock.m_stoppedRiskyProbe),
      m_probeWait(sock.m_probeWait),
      m_prevProbeTooHigh(sock.m_prevProbeTooHigh),
      m_bwProbeSamples(sock.m_bwProbeSamples),
      m_inflightHeadroom(sock.m_inflightHeadroom)
{
    NS_LOG_FUNCTION(this);
    m_bwHi[0] = sock.m_bwHi[0];
    m_bwHi[1] = sock.m_bwHi[1];
}

const char* const TcpBbrV2::BbrModeName[BBR_PROBE_RTT + 1] = {
    "BBR_STARTUP",
    "BBR_DRAIN",
    "BBR_PROBE_BW",
    "BBR_PROBE_RTT",
};

void
TcpBbrV2::SetStream(uint32_t stream)
{
    NS_LOG_FUNCTION(this << stream);
    m_uv->SetStream(stream);
}

void
TcpBbrV2::InitRoundCounting()
{
    NS_LOG_FUNCTION(this);
    m_nextRoundDelivered = 0;
    m_roundStart = false;
    m_roundCount = 0;
}

void
TcpBbrV2::InitFullPipe()
{
    NS_LOG_FUNCTION(this);
    m_isPipeFilled = false;
    m_fullBandwidth = 0;
    m_fullBandwidthCount = 0;
}

void
TcpBbrV2::InitPacingRate(Ptr<TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);

    if (!tcb->m_pacing)
    {
        NS_LOG_WARN("BBR must use pacing");
        tcb->m_pacing = true;
    }

    Time rtt;
    if (tcb->m_minRtt != Time::Max())
    {
        rtt = MilliSeconds(std::max<long int>(tcb->m_minRtt.GetMilliSeconds(), 1));
        m_hasSeenRtt = true;
    }
    else
    {
        rtt = MilliSeconds(1);
    }

    DataRate nominalBandwidth(tcb->m_cWnd * 8 / rtt.GetSeconds());
    tcb->m_pacingRate = DataRate(m_pacingGain * nominalBandwidth.GetBitRate());
    m_maxBwFilter = MaxBandwidthFilter_t(m_bandwidthWindowLength,
                                         DataRate(tcb->m_cWnd * 8 / rtt.GetSeconds()),
                                         0);
}

void
TcpBbrV2::EnterStartup()
{
    NS_LOG_FUNCTION(this);
    SetBbrState(BbrMode_t::BBR_STARTUP);
    m_pacingGain = m_highGain;
    m_cWndGain = m_highGain;
}

// void
// TcpBbrV2::HandleRestartFromIdle(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs)
// {
//     NS_LOG_FUNCTION(this << tcb << rs);
//     if (tcb->m_bytesInFlight.Get() == 0U && rs.m_isAppLimited)
//     {
//         m_idleRestart = true;
//         if (m_state == BbrMode_t::BBR_PROBE_BW)
//         {
//             SetPacingRate(tcb, 1);
//         }
//     }
// }

void
TcpBbrV2::SetPacingRate(Ptr<TcpSocketState> tcb, double gain)
{
    NS_LOG_FUNCTION(this << tcb << gain);
    DataRate rate(gain * GetBandwidth());
    rate *= (1.f - m_pacingMargin);
    rate = std::min(rate, tcb->m_maxPacingRate);

    if (!m_hasSeenRtt && tcb->m_minRtt != Time::Max())
    {
        InitPacingRate(tcb);
    }

    if (m_isPipeFilled || rate > tcb->m_pacingRate)
    {
        tcb->m_pacingRate = rate;
        NS_LOG_DEBUG("Pacing rate updated. New value: " << tcb->m_pacingRate);
    }
}

uint32_t
TcpBbrV2::InFlight(Ptr<TcpSocketState> tcb, double gain)
{
    NS_LOG_FUNCTION(this << tcb << gain);
    if (m_minRtt == Time::Max())
    {
        return tcb->m_initialCWnd * tcb->m_segmentSize;
    }
    double quanta = 3 * m_sendQuantum;
    double estimatedBdp = m_maxBwFilter.GetBest() * m_minRtt / 8.0;

    if (m_state == BbrMode_t::BBR_PROBE_BW && m_cycleIndex == BbrProbeBwPhase_t::BBR_BW_PROBE_UP)
    {
        return (gain * estimatedBdp) + quanta + (2 * tcb->m_segmentSize);
    }
    return (gain * estimatedBdp) + quanta;
}

// void
// TcpBbrV2::AdvanceCyclePhase()
// {
//     NS_LOG_FUNCTION(this);
//     m_cycleStamp = Simulator::Now();
//     m_cycleIndex = (m_cycleIndex + 1) % GAIN_CYCLE_LENGTH;
//     m_pacingGain = PACING_GAIN_CYCLE[m_cycleIndex];
// }

// bool
// TcpBbrV2::IsNextCyclePhase(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs)
// {
//     NS_LOG_FUNCTION(this << tcb << rs);
//     bool isFullLength = (Simulator::Now() - m_cycleStamp) > m_minRtt;
//     if (m_pacingGain == 1)
//     {
//         return isFullLength;
//     }
//     else if (m_pacingGain > 1)
//     {
//         return isFullLength &&
//                (rs.m_bytesLoss > 0 || rs.m_priorInFlight >= InFlight(tcb, m_pacingGain));
//     }
//     else
//     {
//         return isFullLength || rs.m_priorInFlight <= InFlight(tcb, 1);
//     }
// }

// void
// TcpBbrV2::CheckCyclePhase(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs)
// {
//     NS_LOG_FUNCTION(this << tcb << rs);
//     if (m_state == BbrMode_t::BBR_PROBE_BW && IsNextCyclePhase(tcb, rs))
//     {
//         AdvanceCyclePhase();
//     }
// }

void
TcpBbrV2::UpdateCyclePhase(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs)
{
    NS_LOG_FUNCTION(this << tcb << rs);
    if (!m_isPipeFilled || AdaptUpperBounds(tcb, rs) || m_state != BbrMode_t::BBR_PROBE_BW)
    {
        return;
    }

    bool isRisky = false;
    bool isQueuing = false;
    switch (m_cycleIndex)
    {
        case BBR_BW_PROBE_CRUISE:
            if (CheckTimeToProbeBw(tcb))
            {
                return;
            }
            break;
        case BBR_BW_PROBE_REFILL:
            if (m_roundStart)
            {
                m_bwProbeSamples = true;
                StartBwProbeUp(tcb);
            }
            break;
        case BBR_BW_PROBE_UP:
            if (m_prevProbeTooHigh && rs.m_priorInFlight >= m_inflightHi)
            {
                m_stoppedRiskyProbe = true;
                isRisky = true;
            } else if (HasElapsedInPhase(m_minRtt.Get()) && rs.m_priorInFlight >= InFlight(tcb, 1.25)) { // TODO
                isQueuing = true;
            }
            if (isRisky || isQueuing)
            {
                m_prevProbeTooHigh = false;
                StartBwProbeDown();
            }
            break;
        case BBR_BW_PROBE_DOWN:
            if (CheckTimeToProbeBw(tcb))
            {
                return;
            }
            if (CheckTimeToCruise(tcb, rs))
            {
                StartBwProbeCruise();
            }
            break;
    }
}

bool
TcpBbrV2::CheckTimeToProbeBw(Ptr<TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);
    if (HasElapsedInPhase(m_probeWait))
    {
        StartBwProbeRefill();
        return true;
    }
    return false;
}

bool
TcpBbrV2::CheckTimeToCruise(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs)
{
    NS_LOG_FUNCTION(this << tcb << rs);
    if (rs.m_priorInFlight > InflightWithHeadroom(tcb))
    {
        return false;
    }

    return rs.m_priorInFlight <= InFlight(tcb, 1);
}

void
TcpBbrV2::StartBwProbeRefill()
{
    NS_LOG_FUNCTION(this);
    ResetLowerBounds();
    m_bwProbeUpRounds = 0;
    m_bwProbeUpAcks = 0;
    m_stoppedRiskyProbe = false;
    m_nextRoundDelivered = m_delivered;
    m_cycleStamp = Simulator::Now();
    m_cycleIndex = BbrProbeBwPhase_t::BBR_BW_PROBE_REFILL;
}

void
TcpBbrV2::StartBwProbeUp(Ptr<TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);
    m_nextRoundDelivered = m_delivered;
    m_cycleStamp = Simulator::Now();
    m_cycleIndex = BbrProbeBwPhase_t::BBR_BW_PROBE_UP;
    RaiseInflightHiSlope(tcb);
}

void
TcpBbrV2::StartBwProbeDown()
{
    NS_LOG_FUNCTION(this);
    ResetCongestionSignals();
    m_bwProbeUpCount = static_cast<uint32_t>(-1);
    PickProbeWait();
    m_cycleStamp = Simulator::Now();
    m_nextRoundDelivered = m_delivered;
    m_cycleIndex = BbrProbeBwPhase_t::BBR_BW_PROBE_DOWN;
}

void
TcpBbrV2::StartBwProbeCruise()
{
    NS_LOG_FUNCTION(this);
    if (m_inflightLo != static_cast<uint32_t>(-1))
    {
        m_inflightLo = std::min(m_inflightLo, m_inflightHi);
    }
    m_cycleIndex = BbrProbeBwPhase_t::BBR_BW_PROBE_CRUISE;
}

bool
TcpBbrV2::HasElapsedInPhase(Time interval)
{
    NS_LOG_FUNCTION(this << interval);
    return Simulator::Now() > m_cycleStamp + interval;
}

uint32_t
TcpBbrV2::TargetInflight(Ptr<TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);
    uint32_t bdp = InFlight(tcb, 1);
    return std::min(bdp, tcb->m_cWnd.Get());
}

void
TcpBbrV2::RaiseInflightHiSlope(Ptr<TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);
    uint32_t growthThisRound = 1 << m_bwProbeUpRounds;
    m_bwProbeUpRounds = std::min(m_bwProbeUpRounds + 1, 30u);
    m_bwProbeUpCount = std::max(tcb->m_cWnd.Get() / growthThisRound, 1u);
}

void
TcpBbrV2::PickProbeWait()
{
    NS_LOG_FUNCTION(this);
    m_probeWait = Seconds(m_uv->GetValue(2.0, 3.0));
}

bool
TcpBbrV2::IsProbingBandwidth()
{
    NS_LOG_FUNCTION(this);
    return (m_state == BbrMode_t::BBR_STARTUP) || 
           (m_state == BbrMode_t::BBR_PROBE_BW &&
           (m_cycleIndex == BbrProbeBwPhase_t::BBR_BW_PROBE_REFILL ||
           m_cycleIndex == BbrProbeBwPhase_t::BBR_BW_PROBE_UP));
}

uint32_t
TcpBbrV2::InflightWithHeadroom(Ptr<TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);
    if (m_inflightHi == static_cast<uint32_t>(-1))
    {
        return static_cast<uint32_t>(-1);
    }

    uint32_t headroom = std::max(1u, static_cast<uint32_t>(m_inflightHi * m_inflightHeadroom));
    return std::max(m_inflightHi - headroom, m_minPipeCwnd);
}

void
TcpBbrV2::BoundCwndForInfligthModel(Ptr<TcpSocketState> tcb)
{
    if (!m_isInitialized)
    {
        return;
    }

    uint32_t cap = static_cast<uint32_t>(-1);
    if (m_state == BbrMode_t::BBR_PROBE_BW && m_cycleIndex != BbrProbeBwPhase_t::BBR_BW_PROBE_CRUISE)
    {
        cap = m_inflightHi;
    }
    else if (m_state == BbrMode_t::BBR_PROBE_RTT || 
            (m_state == BbrMode_t::BBR_PROBE_BW && 
            m_cycleIndex == BbrProbeBwPhase_t::BBR_BW_PROBE_CRUISE))
    {
        cap = InflightWithHeadroom(tcb);
    }

    cap = std::min(cap, m_inflightLo);
    cap = std::max(cap, m_minPipeCwnd);

    tcb->m_cWnd = std::min(tcb->m_cWnd.Get(), cap);
}

void
TcpBbrV2::CheckFullPipe(const TcpRateOps::TcpRateSample& rs)
{
    NS_LOG_FUNCTION(this << rs);
    if (m_isPipeFilled || !m_roundStart || rs.m_isAppLimited)
    {
        return;
    }

    /* Check if Bottleneck bandwidth is still growing*/
    if (m_maxBwFilter.GetBest().GetBitRate() >= m_fullBandwidth.GetBitRate() * 1.25)
    {
        m_fullBandwidth = m_maxBwFilter.GetBest();
        m_fullBandwidthCount = 0;
        return;
    }

    m_fullBandwidthCount++;
    if (m_fullBandwidthCount >= 3)
    {
        NS_LOG_DEBUG("Pipe filled");
        m_isPipeFilled = true;
    }
}

void
TcpBbrV2::EnterDrain()
{
    NS_LOG_FUNCTION(this);
    SetBbrState(BbrMode_t::BBR_DRAIN);
    m_pacingGain = 1.0 / m_highGain;
    m_cWndGain = m_highGain;
}

void
TcpBbrV2::EnterProbeBW()
{
    NS_LOG_FUNCTION(this);
    SetBbrState(BbrMode_t::BBR_PROBE_BW);
    m_pacingGain = 1;
    m_cWndGain = 2;
    // m_cycleIndex = GAIN_CYCLE_LENGTH - 1 - (int)m_uv->GetValue(0, 6);
    // AdvanceCyclePhase();
}

void
TcpBbrV2::CheckDrain(Ptr<TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);
    if (m_state == BbrMode_t::BBR_STARTUP && m_isPipeFilled)
    {
        EnterDrain();
        tcb->m_ssThresh = InFlight(tcb, 1);
        ResetCongestionSignals();
    }

    if (m_state == BbrMode_t::BBR_DRAIN && tcb->m_bytesInFlight <= InFlight(tcb, 1))
    {
        EnterProbeBW();
        StartBwProbeDown();
    }
}

void
TcpBbrV2::UpdateRTprop(Ptr<TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);
    m_probeRttMinExpired = Simulator::Now() > (m_probeRttMinStamp + m_probeRttMinFilterLen);
    if (tcb->m_lastRtt >= Seconds(0) && (tcb->m_lastRtt <= m_probeRttMin || m_probeRttMinExpired))
    {
        m_probeRttMin = tcb->m_lastRtt;
        m_probeRttMinStamp = Simulator::Now();
    }

    m_minRttExpired = Simulator::Now() > (m_minRttStamp + m_minRttFilterLen);
    if (tcb->m_lastRtt >= Seconds(0) && (tcb->m_lastRtt <= m_minRtt || m_minRttExpired))
    {
        m_minRtt = tcb->m_lastRtt;
        m_minRttStamp = Simulator::Now();
    }
}

void
TcpBbrV2::EnterProbeRTT()
{
    NS_LOG_FUNCTION(this);
    SetBbrState(BbrMode_t::BBR_PROBE_RTT);
    m_pacingGain = 1;
    m_cWndGain = 1;
}

void
TcpBbrV2::SaveCwnd(Ptr<const TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);
    if (tcb->m_congState != TcpSocketState::CA_RECOVERY && m_state != BbrMode_t::BBR_PROBE_RTT)
    {
        m_priorCwnd = tcb->m_cWnd;
    }
    else
    {
        m_priorCwnd = std::max(m_priorCwnd, tcb->m_cWnd.Get());
    }
}

void
TcpBbrV2::RestoreCwnd(Ptr<TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);
    tcb->m_cWnd = std::max(m_priorCwnd, tcb->m_cWnd.Get());
}

void
TcpBbrV2::ExitProbeRTT()
{
    NS_LOG_FUNCTION(this);
    if (m_isPipeFilled)
    {
        EnterProbeBW();
        StartBwProbeDown();
        StartBwProbeCruise();
    }
    else
    {
        EnterStartup();
    }
}

void
TcpBbrV2::HandleProbeRTT(Ptr<TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);

    uint32_t totalBytes = m_delivered + tcb->m_bytesInFlight.Get();
    m_appLimited = (totalBytes > 0 ? totalBytes : 1);

    if (m_probeRttDoneStamp == Seconds(0) && tcb->m_bytesInFlight <= ProbeRTTCwnd(tcb))
    {
        m_probeRttDoneStamp = Simulator::Now() + m_probeRttDuration;
        m_probeRttRoundDone = false;
        m_nextRoundDelivered = m_delivered;
    }
    else if (m_probeRttDoneStamp != Seconds(0))
    {
        if (m_roundStart)
        {
            m_probeRttRoundDone = true;
        }
        if (m_probeRttRoundDone && Simulator::Now() > m_probeRttDoneStamp)
        {
            m_minRttStamp = Simulator::Now();
            RestoreCwnd(tcb);
            ExitProbeRTT();
        }
    }
}

void
TcpBbrV2::CheckProbeRTT(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs)
{
    NS_LOG_FUNCTION(this << tcb);
    if (m_state != BbrMode_t::BBR_PROBE_RTT && m_probeRttMinExpired && !m_idleRestart)
    {
        EnterProbeRTT();
        SaveCwnd(tcb);
        m_probeRttDoneStamp = Seconds(0);
        m_nextRoundDelivered = m_delivered;
    }

    if (m_state == BbrMode_t::BBR_PROBE_RTT)
    {
        HandleProbeRTT(tcb);
    }

    if (rs.m_delivered)
    {
        m_idleRestart = false;
    }
}

void
TcpBbrV2::SetSendQuantum(Ptr<TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);
    m_sendQuantum = 1 * tcb->m_segmentSize;
}

void
TcpBbrV2::UpdateTargetCwnd(Ptr<TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);
    m_targetCWnd = InFlight(tcb, m_cWndGain) + AckAggregationCwnd();
}

uint32_t
TcpBbrV2::AckAggregationCwnd()
{
    NS_LOG_FUNCTION(this);
    uint32_t maxAggrBytes; // MaxBW * 0.1 secs
    uint32_t aggrCwndBytes = 0;

    if (m_extraAckedGain && m_isPipeFilled)
    {
        maxAggrBytes = m_maxBwFilter.GetBest().GetBitRate() / (10 * 8);
        aggrCwndBytes = m_extraAckedGain * std::max(m_extraAcked[0], m_extraAcked[1]);
        aggrCwndBytes = std::min(aggrCwndBytes, maxAggrBytes);
    }
    return aggrCwndBytes;
}

void
TcpBbrV2::UpdateAckAggregation(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs)
{
    NS_LOG_FUNCTION(this << tcb << rs);
    uint32_t expectedAcked;
    uint32_t extraAck;
    uint32_t epochProp;

    if (!m_extraAckedGain || rs.m_ackedSacked <= 0 || rs.m_delivered < 0)
    {
        return;
    }

    if (m_roundStart)
    {
        m_extraAckedWinRtt = std::min<uint32_t>(31, m_extraAckedWinRtt + 1);
        if (m_extraAckedWinRtt >= m_extraAckedWinRttLength)
        {
            m_extraAckedWinRtt = 0;
            m_extraAckedIdx = m_extraAckedIdx ? 0 : 1;
            m_extraAcked[m_extraAckedIdx] = 0;
        }
    }

    epochProp = Simulator::Now().GetSeconds() - m_ackEpochTime.GetSeconds();
    expectedAcked = m_maxBwFilter.GetBest().GetBitRate() * epochProp / 8;

    if (m_ackEpochAcked <= expectedAcked ||
        (m_ackEpochAcked + rs.m_ackedSacked >= m_ackEpochAckedResetThresh))
    {
        m_ackEpochAcked = 0;
        m_ackEpochTime = Simulator::Now();
        expectedAcked = 0;
    }

    m_ackEpochAcked = m_ackEpochAcked + rs.m_ackedSacked;
    extraAck = m_ackEpochAcked - expectedAcked;
    extraAck = std::min(extraAck, tcb->m_cWnd.Get());

    if (extraAck > m_extraAcked[m_extraAckedIdx])
    {
        m_extraAcked[m_extraAckedIdx] = extraAck;
    }
}

void
TcpBbrV2::ResetCongestionSignals()
{
    NS_LOG_FUNCTION(this);

    m_lossInRound = false;
    m_ecnInRound = false;
    m_lossInCycle = false;
    m_ecnInCycle = false;
    m_bwLatest = 0;
    m_inflightLatest = 0;
}

void
TcpBbrV2::ResetLowerBounds()
{
    NS_LOG_FUNCTION(this);

    m_bwLo = static_cast<uint64_t>(-1);
    m_inflightLo = static_cast<uint32_t>(-1);
}

void
TcpBbrV2::UpdateCongestionSignals(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs)
{
    NS_LOG_FUNCTION(this << tcb << rs);
    uint64_t sampleBw = CalculateBwSample(rs);

    m_lossRoundStart = false;
    if (rs.m_interval <= Seconds(0) || rs.m_ackedSacked == 0) {
        return;
    }

    if (!rs.m_isAppLimited || sampleBw >= MaxBw())
    {
        TakeBwHiSample(sampleBw);
    }

    m_lossInRound |= (rs.m_bytesLoss > 0);

    m_bwLatest = std::max(m_bwLatest, sampleBw);
    if (rs.m_delivered != -1)
    {
        m_inflightLatest = std::max(m_inflightLatest, static_cast<uint32_t>(rs.m_delivered));
    }

    if (rs.m_priorDelivered >= m_lossRoundDelivered) {
        m_lossRoundDelivered = m_delivered;
        m_lossRoundStart = true;
        AdaptLowerBounds(tcb);

        m_lossInRound = false;
        m_ecnInRound = false;
        m_bwLatest = sampleBw;
        m_inflightLatest = rs.m_delivered;
    }
}

void
TcpBbrV2::CheckLossTooHighInStartup(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs)
{
    NS_LOG_FUNCTION(this << tcb << rs);
    if (m_isPipeFilled)
    {
        return;
    }

    if (rs.m_bytesLoss > 0)
    {
        m_lossEventsInRound++;
    }

    if (m_fullLossCount > 0 && m_lossRoundStart &&
        tcb->m_congState == TcpSocketState::CA_RECOVERY &&
        m_lossEventsInRound >= m_fullLossCount &&
        IsInflightTooHigh(tcb, rs))
    {
        HandleQueueTooHighInStartup(tcb);
        return;
    }
    if (m_lossRoundStart)
    {
        m_lossEventsInRound = 0;
    }
}

void
TcpBbrV2::CheckEcnTooHighInStartup(Ptr<TcpSocketState> tcb, double ceRatio)
{
    NS_LOG_FUNCTION(this << tcb << ceRatio);

    if (m_isPipeFilled || !m_ecnEligible || !m_fullEcnCount || !m_ecnThresh)
    {
        return;
    }

    if (ceRatio >= m_ecnThresh)
    {
        m_startupEcnRounds++;
    }
    else
    {
        m_startupEcnRounds = 0;
    }

    if (m_startupEcnRounds >= m_fullEcnCount)
    {
        HandleQueueTooHighInStartup(tcb);
        return;
    }
}

double
TcpBbrV2::UpdateEcnAlpha(Ptr<TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);

    if (m_ecnFactor == 0)
    {
        return -1;
    }

    uint32_t delivered = m_delivered - m_alphaLastDelivered;
    uint32_t deliveredCe = m_deliveredCe - m_alphaLastDeliveredCe;
    if (delivered == 0)
    {
        return -1;
    }

    if (!m_ecnEligible && (m_minRtt <= m_ecnMaxRtt || m_ecnMaxRtt == Seconds(0)))
    {
        m_ecnEligible = true;
    }

    double ceRatio = static_cast<double>(deliveredCe) / delivered;
    double alpha = (1 - m_ecnAlphaGain) * m_ecnAlpha;
    alpha += m_ecnAlphaGain * ceRatio;
    m_ecnAlpha = std::min(alpha, 1.0);

    m_alphaLastDelivered = m_delivered;
    m_alphaLastDeliveredCe = m_deliveredCe;

    CheckEcnTooHighInStartup(tcb, ceRatio);
    return ceRatio;
}

void
TcpBbrV2::HandleQueueTooHighInStartup(Ptr<TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);
    m_isPipeFilled = true;
    m_inflightHi = InFlight(tcb, 1.0);
}

void
TcpBbrV2::ProbeInflightHiUpward(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs)
{
    NS_LOG_FUNCTION(this << tcb << rs);

    if (!tcb->m_isCwndLimited || tcb->m_cWnd < m_inflightHi)
    {
        m_bwProbeUpAcks = 0;
        return;
    }

    m_bwProbeUpAcks += rs.m_ackedSacked;
    if (m_bwProbeUpAcks >= m_bwProbeUpCount)
    {
        uint32_t delta = m_bwProbeUpAcks / m_bwProbeUpCount;
        m_bwProbeUpAcks -= delta * m_bwProbeUpCount;
        m_inflightHi += delta;
    }
    if (m_roundStart)
    {
        RaiseInflightHiSlope(tcb);
    }
}

void
TcpBbrV2::HandleInflightTooHigh(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs)
{
    NS_LOG_FUNCTION(this << tcb << rs);
    m_prevProbeTooHigh = true;
    m_bwProbeSamples = false;

    if (!rs.m_isAppLimited)
    {
        m_inflightHi = std::max(
            tcb->m_bytesInFlight.Get(),
            static_cast<uint32_t>(TargetInflight(tcb) * (1 - m_beta))
        );
    }
    
    if (m_state == BbrMode_t::BBR_PROBE_BW && m_cycleIndex == BbrProbeBwPhase_t::BBR_BW_PROBE_UP)
    {
        StartBwProbeDown();
    }
}

bool
TcpBbrV2::AdaptUpperBounds(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs)
{
    NS_LOG_FUNCTION(this << tcb << rs);
    if (m_roundStart) // TODO
    {
        m_bwProbeSamples = false;
        if (m_state == BbrMode_t::BBR_PROBE_BW && !rs.m_isAppLimited)
        {
            AdvanceBwHiFilter();
        }
        if (m_state == BbrMode_t::BBR_PROBE_BW && m_stoppedRiskyProbe && !m_prevProbeTooHigh)
        {
            StartBwProbeRefill();
            return true;
        }
    }

    if (IsInflightTooHigh(tcb, rs))
    {
        if (m_bwProbeSamples)
        {
            HandleInflightTooHigh(tcb, rs);
        }
    }
    else
    {
        if (m_inflightHi == static_cast<uint32_t>(-1))
        {
            return false;
        }

        if (tcb->m_bytesInFlight > m_inflightHi)
        {
            m_inflightHi = tcb->m_bytesInFlight;
        }

        if (m_state == BbrMode_t::BBR_PROBE_BW && m_cycleIndex == BbrProbeBwPhase_t::BBR_BW_PROBE_UP)
        {
            ProbeInflightHiUpward(tcb, rs);
        }
    }

    return false;
}

void
TcpBbrV2::AdaptLowerBounds(Ptr<TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);
    if (IsProbingBandwidth())
    {
        return;
    }

    uint32_t ecnInflightLo = static_cast<uint32_t>(-1);
    if (m_ecnInRound && m_ecnEligible && m_ecnFactor != 0)
    {
        double ecnCut = 1 - m_ecnAlpha * m_ecnFactor;
        if (m_inflightLo == static_cast<uint32_t>(-1))
        {
            m_inflightLo = tcb->m_cWnd;
        }
        ecnInflightLo = m_inflightLo * ecnCut;
    }

    if (m_lossInRound)
    {
        if (m_bwLo == static_cast<uint64_t>(-1))
        {
            m_bwLo = MaxBw();
        }
        if (m_inflightLo == static_cast<uint32_t>(-1))
        {
            m_inflightLo = tcb->m_cWnd;
        }
        m_bwLo = std::max(m_bwLatest, static_cast<uint64_t>(m_bwLo * (1 - m_beta)));
        m_inflightLo = std::max(m_inflightLatest, static_cast<uint32_t>(m_inflightLo * (1 - m_beta)));
    }

    m_inflightLo = std::min(m_inflightLo, ecnInflightLo);
}

bool
TcpBbrV2::IsInflightTooHigh(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs)
{
    NS_LOG_FUNCTION(this << tcb << rs);

    if (rs.m_lost > 0 && tcb->m_bytesInFlight > 0u)
    {
        uint32_t lossThresh = static_cast<uint32_t>(tcb->m_bytesInFlight * m_lossThresh);
        if (rs.m_lost > static_cast<int32_t>(lossThresh))
        {
            return true;
        }
    }

    if (rs.m_deliveredCe > 0 && rs.m_delivered > 0 && m_ecnEligible && m_ecnThresh > 0)
    {
        uint32_t ecnThresh = static_cast<uint32_t>(rs.m_delivered * m_ecnThresh);
        if (rs.m_deliveredCe >= static_cast<int32_t>(ecnThresh))
        {
            return true;
        }
    }

    return false;
}

uint64_t
TcpBbrV2::MaxBw()
{
    NS_LOG_FUNCTION(this);
    return std::max(m_bwHi[0], m_bwHi[1]);
}

void
TcpBbrV2::TakeBwHiSample(uint64_t bw)
{
    NS_LOG_FUNCTION(this << bw);
    m_bwHi[1] = std::max(bw, m_bwHi[1]);
}

void
TcpBbrV2::AdvanceBwHiFilter()
{
    NS_LOG_FUNCTION(this);
    if (m_bwHi[1] == 0)
    {
        return;
    }
    m_bwHi[0] = m_bwHi[1];
    m_bwHi[1] = 0;
}

uint64_t
TcpBbrV2::CalculateBwSample(const TcpRateOps::TcpRateSample& rs)
{
    NS_LOG_FUNCTION(this << rs);

    uint32_t deliveredBits = 8 * rs.m_delivered;
    uint32_t interval = rs.m_interval.GetSeconds();
    if (deliveredBits >= 0 && interval > 0)
    {
        return deliveredBits / interval + (deliveredBits % interval != 0);
    }
    return 0;
}

uint64_t
TcpBbrV2::GetBandwidth()
{
    NS_LOG_FUNCTION(this);
    return std::min(m_maxBwFilter.GetBest().GetBitRate(), m_bwLo);
}

bool
TcpBbrV2::ModulateCwndForRecovery(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs)
{
    NS_LOG_FUNCTION(this << tcb << rs);
    if (rs.m_bytesLoss > 0)
    {
        tcb->m_cWnd =
            std::max((int)tcb->m_cWnd.Get() - (int)rs.m_bytesLoss, (int)tcb->m_segmentSize);
    }

    if (m_packetConservation)
    {
        tcb->m_cWnd = std::max(tcb->m_cWnd.Get(), tcb->m_bytesInFlight.Get() + rs.m_ackedSacked);
        return true;
    }
    return false;
}

void
TcpBbrV2::ModulateCwndForProbeRTT(Ptr<TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);
    if (m_state == BbrMode_t::BBR_PROBE_RTT)
    {
        tcb->m_cWnd = std::min(tcb->m_cWnd.Get(), ProbeRTTCwnd(tcb));
    }
}

uint32_t
TcpBbrV2::ProbeRTTCwnd(Ptr<TcpSocketState> tcb)
{
    NS_LOG_FUNCTION(this << tcb);

    if (m_probeRttCwndGain == 0)
    {
        return m_minPipeCwnd;
    }
    return std::max(m_minPipeCwnd, GetBDP(tcb, m_probeRttCwndGain));
}

void
TcpBbrV2::SetCwnd(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs)
{
    NS_LOG_FUNCTION(this << tcb << rs);

    if (!rs.m_ackedSacked)
    {
        goto done;
    }

    // if (tcb->m_congState == TcpSocketState::CA_RECOVERY) // TODO: remove
    // {
    //     if (ModulateCwndForRecovery(tcb, rs))
    //     {
    //         goto done;
    //     }
    // }

    UpdateTargetCwnd(tcb);

    if (m_isPipeFilled)
    {
        tcb->m_cWnd = std::min(tcb->m_cWnd.Get() + (uint32_t)rs.m_ackedSacked, m_targetCWnd);
    }
    else if (tcb->m_cWnd < m_targetCWnd || m_delivered < 2 * tcb->m_initialCWnd * tcb->m_segmentSize)
    {
        tcb->m_cWnd = tcb->m_cWnd.Get() + rs.m_ackedSacked;
    }
    tcb->m_cWnd = std::max(tcb->m_cWnd.Get(), m_minPipeCwnd);
    NS_LOG_DEBUG("Congestion window updated. New value:" << tcb->m_cWnd);

done:
    ModulateCwndForProbeRTT(tcb);
}

void
TcpBbrV2::UpdateRound(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs)
{
    NS_LOG_FUNCTION(this << tcb << rs);
    if (rs.m_interval > Seconds(0) && rs.m_priorDelivered >= m_nextRoundDelivered)
    {
        m_nextRoundDelivered = m_delivered;
        m_roundCount++;
        m_roundStart = true;
        m_packetConservation = false; // TODO: remove
    }
    else
    {
        m_roundStart = false;
    }
}

void
TcpBbrV2::UpdateBottleneckBandwidth(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs)
{
    NS_LOG_FUNCTION(this << tcb << rs);

    if (rs.m_delivered < 0 || rs.m_interval.IsZero())
    {
        return;
    }

    if (rs.m_deliveryRate >= m_maxBwFilter.GetBest() || !rs.m_isAppLimited)
    {
        m_maxBwFilter.Update(rs.m_deliveryRate, m_roundCount);
    }
}

void
TcpBbrV2::UpdateModelAndState(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs)
{
    NS_LOG_FUNCTION(this << tcb << rs);
    UpdateCongestionSignals(tcb, rs);
    UpdateAckAggregation(tcb, rs);
    CheckLossTooHighInStartup(tcb, rs);
    CheckFullPipe(rs);
    CheckDrain(tcb);
    UpdateCyclePhase(tcb, rs);
    UpdateRTprop(tcb);
    CheckProbeRTT(tcb, rs);
}

void
TcpBbrV2::UpdateControlParameters(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs)
{
    NS_LOG_FUNCTION(this << tcb << rs);
    SetPacingRate(tcb, m_pacingGain);
    SetSendQuantum(tcb);
    SetCwnd(tcb, rs); // TODO
}

void
TcpBbrV2::SetBbrState(BbrMode_t mode)
{
    NS_LOG_FUNCTION(this << mode);
    NS_LOG_DEBUG(Simulator::Now() << " Changing from " << BbrModeName[m_state] << " to "
                                  << BbrModeName[mode]);
    m_state = mode;
}

uint32_t
TcpBbrV2::GetBbrState()
{
    NS_LOG_FUNCTION(this);
    return m_state;
}

double
TcpBbrV2::GetCwndGain()
{
    NS_LOG_FUNCTION(this);
    return m_cWndGain;
}

uint32_t
TcpBbrV2::GetBDP(Ptr<TcpSocketState> tcb, double gain)
{
    NS_LOG_FUNCTION(this << tcb << gain);
    if (m_minRtt == Time::Max())
    {
        return tcb->m_initialCWnd * tcb->m_segmentSize;
    }

    double estimatedBdp = m_maxBwFilter.GetBest() * m_minRtt / 8.0;
    return estimatedBdp * gain;
}

double
TcpBbrV2::GetPacingGain()
{
    NS_LOG_FUNCTION(this);
    return m_pacingGain;
}

std::string
TcpBbrV2::GetName() const
{
    return "TcpBbrV2";
}

bool
TcpBbrV2::HasCongControl() const
{
    NS_LOG_FUNCTION(this);
    return true;
}

void
TcpBbrV2::CongControl(Ptr<TcpSocketState> tcb,
                    const TcpRateOps::TcpRateConnection& rc,
                    const TcpRateOps::TcpRateSample& rs)
{
    NS_LOG_FUNCTION(this << tcb << rs);
    m_delivered = rc.m_delivered;
    m_deliveredCe = rc.m_deliveredCe;
    m_lost = rc.m_lost;

    UpdateRound(tcb, rs);
    if (m_roundStart)
    {
        UpdateEcnAlpha(tcb);
    }

    m_ecnInRound |= tcb->m_ecnState == TcpSocketState::ECN_ECE_RCVD;

    UpdateBottleneckBandwidth(tcb, rs);
    UpdateModelAndState(tcb, rs);
    UpdateControlParameters(tcb, rs);
    BoundCwndForInfligthModel(tcb);

    m_lossInCycle |= rs.m_lost > 0;
    m_ecnInCycle |= rs.m_deliveredCe > 0;
}

void
TcpBbrV2::CongestionStateSet(Ptr<TcpSocketState> tcb, const TcpSocketState::TcpCongState_t newState)
{
    NS_LOG_FUNCTION(this << tcb << newState);
    if (newState == TcpSocketState::CA_OPEN && !m_isInitialized)
    {
        NS_LOG_DEBUG("CongestionStateSet triggered to CA_OPEN :: " << newState);
        m_minRtt = tcb->m_srtt.Get() != Time::Max() ? tcb->m_srtt.Get() : Time::Max();
        m_minRttStamp = Simulator::Now();
        m_priorCwnd = tcb->m_cWnd;
        tcb->m_ssThresh = tcb->m_initialSsThresh;
        m_targetCWnd = tcb->m_cWnd;
        m_minPipeCwnd = 4 * tcb->m_segmentSize;
        m_sendQuantum = 1 * tcb->m_segmentSize;

        InitRoundCounting();
        InitFullPipe();
        EnterStartup();
        InitPacingRate(tcb);
        m_ackEpochTime = Simulator::Now();
        m_extraAckedWinRtt = 0;
        m_extraAckedIdx = 0;
        m_ackEpochAcked = 0;
        m_extraAcked[0] = 0;
        m_extraAcked[1] = 0;
        m_isInitialized = true;
    }
    else if (newState == TcpSocketState::CA_LOSS)
    {
        NS_LOG_DEBUG("CongestionStateSet triggered to CA_LOSS :: " << newState);
        SaveCwnd(tcb);
        m_roundStart = true;
    }
    else if (newState == TcpSocketState::CA_RECOVERY)
    {
        NS_LOG_DEBUG("CongestionStateSet triggered to CA_RECOVERY :: " << newState);
        SaveCwnd(tcb);
        tcb->m_cWnd =
            tcb->m_bytesInFlight.Get() + std::max(tcb->m_lastAckedSackedBytes, tcb->m_segmentSize);
        m_packetConservation = true;
    }
}

void
TcpBbrV2::CwndEvent(Ptr<TcpSocketState> tcb, const TcpSocketState::TcpCAEvent_t event)
{
    NS_LOG_FUNCTION(this << tcb << event);
    if (event == TcpSocketState::CA_EVENT_COMPLETE_CWR)
    {
        NS_LOG_DEBUG("CwndEvent triggered to CA_EVENT_COMPLETE_CWR :: " << event);
        m_packetConservation = false;
        RestoreCwnd(tcb);
    }
    else if (event == TcpSocketState::CA_EVENT_TX_START && m_appLimited)
    {
        NS_LOG_DEBUG("CwndEvent triggered to CA_EVENT_TX_START :: " << event);
        m_idleRestart = true;
        m_ackEpochTime = Simulator::Now();
        m_ackEpochAcked = 0;
        if (m_state == BbrMode_t::BBR_PROBE_BW)
        {
            SetPacingRate(tcb, 1);
        }
        else if (m_state == BbrMode_t::BBR_PROBE_RTT)
        {
            if (m_probeRttRoundDone && Simulator::Now() > m_probeRttDoneStamp)
            {
                m_minRttStamp = Simulator::Now();
                RestoreCwnd(tcb);
                ExitProbeRTT();
            }
        }
    }
}

uint32_t
TcpBbrV2::GetSsThresh(Ptr<const TcpSocketState> tcb, uint32_t bytesInFlight)
{
    NS_LOG_FUNCTION(this << tcb << bytesInFlight);
    SaveCwnd(tcb);
    return tcb->m_ssThresh;
}

Ptr<TcpCongestionOps>
TcpBbrV2::Fork()
{
    return CopyObject<TcpBbrV2>(this);
}

} // namespace ns3
