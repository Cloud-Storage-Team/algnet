/*
 * Copyright (c) 2018 NITK Surathkal
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 * Authors: Vivek Jain <jain.vivek.anand@gmail.com>
 *          Viyom Mittal <viyommittal@gmail.com>
 *          Mohit P. Tahiliani <tahiliani@nitk.edu.in>
 */

#ifndef TCPBBRV2_H
#define TCPBBRV2_H

#include "tcp-congestion-ops.h"
#include "windowed-filter.h"

#include "ns3/data-rate.h"
#include "ns3/random-variable-stream.h"
#include "ns3/traced-value.h"

class TcpBbrV2CheckGainValuesTest;

namespace ns3
{

/**
 * \ingroup congestionOps
 *
 * \brief BBR congestion control algorithm
 *
 * This class implement the BBR (Bottleneck Bandwidth and Round-trip propagation time)
 * congestion control type.
 */
class TcpBbrV2 : public TcpCongestionOps
{
  public:
    /**
     * \brief The number of phases in the BBR ProbeBW gain cycle.
     */
    static const uint8_t GAIN_CYCLE_LENGTH = 8;

    /**
     * \brief BBR uses an eight-phase cycle with the given pacing_gain value
     * in the BBR ProbeBW gain cycle.
     */
    const static double PACING_GAIN_CYCLE[];
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId();

    /**
     * \brief Constructor
     */
    TcpBbrV2();

    /**
     * Copy constructor.
     * \param sock The socket to copy from.
     */
    TcpBbrV2(const TcpBbrV2& sock);

    /**
     * \brief BBR has the following 4 modes for deciding how fast to send:
     */
    enum BbrMode_t
    {
        BBR_STARTUP,   /**< Ramp up sending rate rapidly to fill pipe */
        BBR_DRAIN,     /**< Drain any queue created during startup */
        BBR_PROBE_BW,  /**< Discover, share bw: pace around estimated bw */
        BBR_PROBE_RTT, /**< Cut inflight to min to probe min_rtt */
    };

    typedef WindowedFilter<DataRate,
                           MaxFilter<DataRate>,
                           uint32_t,
                           uint32_t>
        MaxBandwidthFilter_t; //!< Definition of max bandwidth filter.

    /**
     * \brief Literal names of BBR mode for use in log messages
     */
    static const char* const BbrModeName[BBR_PROBE_RTT + 1];

    /**
     * Assign a fixed random variable stream number to the random variables
     * used by this model.
     *
     * \param stream first stream index to use
     */
    virtual void SetStream(uint32_t stream);

    std::string GetName() const override;
    bool HasCongControl() const override;
    void CongControl(Ptr<TcpSocketState> tcb,
                     const TcpRateOps::TcpRateConnection& rc,
                     const TcpRateOps::TcpRateSample& rs) override;
    void CongestionStateSet(Ptr<TcpSocketState> tcb,
                            const TcpSocketState::TcpCongState_t newState) override;
    void CwndEvent(Ptr<TcpSocketState> tcb, const TcpSocketState::TcpCAEvent_t event) override;
    uint32_t GetSsThresh(Ptr<const TcpSocketState> tcb, uint32_t bytesInFlight) override;
    Ptr<TcpCongestionOps> Fork() override;

  protected:
    /**
     * \brief TcpBbrCheckGainValuesTest friend class (for tests).
     * \relates TcpBbrCheckGainValuesTest
     */
    friend class TcpBbrCheckGainValuesTest;

    /**
     * \brief Advances pacing gain using cycle gain algorithm, while in BBR_PROBE_BW state
     */
    void AdvanceCyclePhase();

    /**
     * \brief Checks whether to advance pacing gain in BBR_PROBE_BW state,
     *  and if allowed calls AdvanceCyclePhase ()
     * \param tcb the socket state.
     * \param rs rate sample.
     */
    void CheckCyclePhase(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs);

    /**
     * \brief Checks whether its time to enter BBR_DRAIN or BBR_PROBE_BW state
     * \param tcb the socket state.
     */
    void CheckDrain(Ptr<TcpSocketState> tcb);

    /**
     * \brief Identifies whether pipe or BDP is already full
     * \param rs rate sample.
     */
    void CheckFullPipe(const TcpRateOps::TcpRateSample& rs);

    /**
     * \brief This method handles the steps related to the ProbeRTT state
     * \param tcb the socket state.
     * \param rs rate sample.
     */
    void CheckProbeRTT(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs);

    /**
     * \brief Updates variables specific to BBR_DRAIN state
     */
    void EnterDrain();

    /**
     * \brief Updates variables specific to BBR_PROBE_BW state
     */
    void EnterProbeBW();

    /**
     * \brief Updates variables specific to BBR_PROBE_RTT state
     */
    void EnterProbeRTT();

    /**
     * \brief Updates variables specific to BBR_STARTUP state
     */
    void EnterStartup();

    /**
     * \brief Called on exiting from BBR_PROBE_RTT state, it eithers invoke EnterProbeBW () or
     * EnterStartup ()
     */
    void ExitProbeRTT();

    /**
     * \brief Gets BBR state.
     * \return returns BBR state.
     */
    uint32_t GetBbrState();

    /**
     * \brief Calculate bdp based on min RTT and the estimated bottleneck bandwidth.
     * \param tcb the socket state.
     * \param gain cwnd gain.
     * \return reutrns calculated bdp.
     */
    uint32_t GetBDP(Ptr<TcpSocketState> tcb, double gain);

    /**
     * \brief Gets current pacing gain.
     * \return returns current pacing gain.
     */
    double GetPacingGain();

    /**
     * \brief Gets current cwnd gain.
     * \return returns current cwnd gain.
     */
    double GetCwndGain();

    /**
     * \brief Handles the steps for BBR_PROBE_RTT state.
     * \param tcb the socket state.
     */
    void HandleProbeRTT(Ptr<TcpSocketState> tcb);

    /**
     * \brief Updates pacing rate if socket is restarting from idle state.
     * \param tcb the socket state.
     * \param rs rate sample.
     */
    void HandleRestartFromIdle(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs);

    /**
     * \brief Estimates the target value for congestion window
     * \param tcb  the socket state.
     * \param gain cwnd gain.
     * \return returns congestion window based on max bandwidth and min RTT.
     */
    uint32_t InFlight(Ptr<TcpSocketState> tcb, double gain);

    /**
     * \brief Initializes the full pipe estimator.
     */
    void InitFullPipe();

    /**
     * \brief Initializes the pacing rate.
     * \param tcb  the socket state.
     */
    void InitPacingRate(Ptr<TcpSocketState> tcb);

    /**
     * \brief Initializes the round counting related variables.
     */
    void InitRoundCounting();

    /**
     * \brief Checks whether to move to next value of pacing gain while in BBR_PROBE_BW.
     * \param tcb the socket state.
     * \param rs  rate sample.
     * \returns true if want to move to next value otherwise false.
     */
    bool IsNextCyclePhase(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs);

    /**
     * \brief Modulates congestion window in BBR_PROBE_RTT.
     * \param tcb the socket state.
     */
    void ModulateCwndForProbeRTT(Ptr<TcpSocketState> tcb);

    /**
     * \brief Modulates congestion window in CA_RECOVERY.
     * \param tcb the socket state.
     * \param rs rate sample.
     * \return true if congestion window is updated in CA_RECOVERY.
     */
    bool ModulateCwndForRecovery(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs);

    /**
     * \brief Returns the cwnd for PROBE_RTT mode
     * \param tcb the socket state.
     * \return the cwnd for PROBE_RTT mode
     */
    uint32_t ProbeRTTCwnd(Ptr<TcpSocketState> tcb);

    /**
     * \brief Helper to restore the last-known good congestion window
     * \param tcb the socket state.
     */
    void RestoreCwnd(Ptr<TcpSocketState> tcb);

    /**
     * \brief Helper to remember the last-known good congestion window or
     *        the latest congestion window unmodulated by loss recovery or ProbeRTT.
     * \param tcb the socket state.
     */
    void SaveCwnd(Ptr<const TcpSocketState> tcb);

    /**
     * \brief Updates congestion window based on the network model.
     * \param tcb the socket state.
     * \param rs  rate sample
     */
    void SetCwnd(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs);

    /**
     * \brief Updates pacing rate based on network model.
     * \param tcb the socket state.
     * \param gain pacing gain.
     */
    void SetPacingRate(Ptr<TcpSocketState> tcb, double gain);

    /**
     * \brief Updates send quantum based on the network model.
     * \param tcb the socket state.
     */
    void SetSendQuantum(Ptr<TcpSocketState> tcb);

    /**
     * \brief Updates maximum bottleneck.
     * \param tcb the socket state.
     * \param rs rate sample.
     */
    void UpdateBottleneckBandwidth(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs);

    /**
     * \brief Updates control parameters congestion windowm, pacing rate, send quantum.
     * \param tcb the socket state.
     * \param rs rate sample.
     */
    void UpdateControlParameters(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs);

    /**
     * \brief Updates BBR network model (Maximum bandwidth and minimum RTT).
     * \param tcb the socket state.
     * \param rs rate sample.
     */
    void UpdateModelAndState(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs);

    /**
     * \brief Updates round counting related variables.
     * \param tcb the socket state.
     * \param rs rate sample.
     */
    void UpdateRound(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs);

    /**
     * \brief Updates minimum RTT.
     * \param tcb the socket state.
     */
    void UpdateRTprop(Ptr<TcpSocketState> tcb);

    /**
     * \brief Updates target congestion window.
     * \param tcb the socket state.
     */
    void UpdateTargetCwnd(Ptr<TcpSocketState> tcb);

    /**
     * \brief Sets BBR state.
     * \param state BBR state.
     */
    void SetBbrState(BbrMode_t state);

    /**
     * \brief Find Cwnd increment based on ack aggregation.
     * \return uint32_t aggregate cwnd.
     */
    uint32_t AckAggregationCwnd();

    /**
     * \brief Estimates max degree of aggregation.
     * \param tcb the socket state.
     * \param rs rate sample.
     */
    void UpdateAckAggregation(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs);

    /**
     * \brief After bw probing (STARTUP/PROBE_UP), reset signals before entering a state
     * machine phase where we adapt our lower bound based on congestion signals.
     */
    void ResetCongestionSignals();

    /**
     * \brief Reset any short-term lower-bound adaptation to congestion, so that we can
     * push our inflight up.
     */
    void ResetLowerBounds();

    /**
     * \brief Update (most of) our congestion signals: track the recent rate and volume of
     * delivered data, presence of loss, and EWMA degree of ECN marking.
     * \param tcb the socket state.
     * \param rs rate sample.
     */
    void UpdateCongestionSignals(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs, uint32_t sampleBw);

    /**
     * \brief Exit STARTUP based on loss rate > 1% and loss gaps in round >= N. Wait until
     * the end of the round in recovery to get a good estimate of how many packets
     * have been lost, and how many we need to drain with a low pacing rate.
     * \param tcb the socket state.
     * \param rs rate sample.
     */
    void CheckLossTooHighInStartup(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs);

    /**
     * \brief Exit STARTUP upon N consecutive rounds with ECN mark rate > ecn_thresh.
     * \param tcb the socket state.
     * \param ceRatio delivered_ce / delivered.
     */
    void CheckEcnTooHighInStartup(Ptr<TcpSocketState> tcb, double ceRatio);

    /**
     * \brief Update m_ecnAlpha.
     * \param tcb the socket state.
     * \return returns delivered_ce / delivered.
     */
    double UpdateEcnAlpha(Ptr<TcpSocketState> tcb);

    /**
     * \brief Handles STARTUP phase on ECN and loss signals.
     * \param tcb the socket state.
     */
    void HandleQueueTooHighInStartup(Ptr<TcpSocketState> tcb);

    /**
     * \brief In BBR_BW_PROBE_UP, if not seeing high loss/ECN/queue, raise inflight_hi.
     * \param tcb the socket state.
     * \param rs rate sample.
     */
    void ProbeInflightHiUpward(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample &rs);

    /**
     * \brief Loss and/or ECN rate is too high while probing.
     * Adapt (once per bw probe) by cutting inflight_hi and then restarting cycle.
     */
    void HandleInflightTooHigh(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample &rs);

    /**
     * \brief If we're seeing bw and loss samples reflecting our bw probing, adapt
     * using the signals we see. If loss or ECN mark rate gets too high, then adapt
     * m_inflightHi downward. If we're able to push inflight higher without such
     * signals, push higher: adapt m_inflightHi upward.
     * \param tcb the socket state.
     * \param rs rate sample.
     * \return returns true if state transition is decided.
     */
    bool AdaptUpperBounds(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample &rs);

    /**
     * \brief Estimate a short-term lower bound on the capacity available now, based
     * on measurements of the current delivery process and recent history. When we
     * are seeing loss/ECN at times when we are not probing bw, then conservatively
     * move toward flow balance by multiplicatively cutting our short-term
     * estimated safe rate and volume of data (bw_lo and inflight_lo). We use a
     * multiplicative decrease in order to converge to a lower capacity in time
     * logarithmic in the magnitude of the decrease.
     *
     * However, we do not cut our short-term estimates lower than the current rate
     * and volume of delivered data from this round trip, since from the current
     * delivery process we can estimate the measured capacity available now.
     *
     * Anything faster than that approach would knowingly risk high loss, which can
     * cause low bw for Reno/CUBIC and high loss recovery latency for
     * request/response flows using any congestion control.
     */
    void AdaptLowerBounds(Ptr<TcpSocketState> tcb);

    /**
     * Does loss/ECN rate for this sample say inflight is "too high"?
     * \param tcb the socket state.
     * \param rs rate sample.
     * \return returns true if loss/ECN rate say inflight is "too high".
     */
    bool IsInflightTooHigh(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample &rs);

    /**
     * \brief Return the windowed max recent bandwidth sample, in pkts/uS.
     */
    uint32_t MaxBw();

    /**
     * \brief Incorporate a new bw sample into the current window of our max filter.
     */
    void TakeBwHiSample(uint32_t bw);

    /**
     * \brief Keep max of last 1-2 cycles. Each PROBE_BW cycle, flip filter window.
     */
    void AdvanceBwHiFilter();

  private:
    BbrMode_t m_state{BbrMode_t::BBR_STARTUP}; //!< Current state of BBR state machine
    MaxBandwidthFilter_t m_maxBwFilter;        //!< Maximum bandwidth filter
    uint32_t m_bandwidthWindowLength{0}; //!< A constant specifying the length of the BBR.BtlBw max
                                         //!< filter window, default 10 packet-timed round trips.
    TracedValue<double> m_pacingGain{0}; //!< The dynamic pacing gain factor
    TracedValue<double> m_cWndGain{0};   //!< The dynamic congestion window gain factor
    double m_highGain{0};       //!< A constant specifying highest gain factor, default is 2.89
    bool m_isPipeFilled{false}; //!< A boolean that records whether BBR has filled the pipe
    uint32_t m_minPipeCwnd{
        0}; //!< The minimal congestion window value BBR tries to target, default 4 Segment size
    uint32_t m_roundCount{0}; //!< Count of packet-timed round trips
    bool m_roundStart{false}; //!< A boolean that BBR sets to true once per packet-timed round trip
    uint32_t m_nextRoundDelivered{0};           //!< Denotes the end of a packet-timed round trip
    Time m_probeRttDuration{MilliSeconds(200)}; //!< A constant specifying the minimum duration for
                                                //!< which ProbeRTT state, default 200 millisecs
    Time m_probeRtPropStamp{
        Seconds(0)}; //!< The wall clock time at which the current BBR.RTProp sample was obtained.
    Time m_probeRttDoneStamp{Seconds(0)}; //!< Time to exit from BBR_PROBE_RTT state
    bool m_probeRttRoundDone{false};      //!< True when it is time to exit BBR_PROBE_RTT
    double m_probeRttCwndGain{0.5};       //!< Cwnd to BDP proportion in PROBE_RTT mode scaled by BBR_UNIT. Default: 50%.
    bool m_packetConservation{false};     //!< Enable/Disable packet conservation mode
    uint32_t m_priorCwnd{0};              //!< The last-known good congestion window
    bool m_idleRestart{false};            //!< When restarting from idle, set it true
    uint32_t m_targetCWnd{0}; //!< Target value for congestion window, adapted to the estimated BDP
    DataRate m_fullBandwidth{0};      //!< Value of full bandwidth recorded
    uint32_t m_fullBandwidthCount{0}; //!< Count of full bandwidth recorded consistently
    TracedValue<Time> m_minRtt{
        Time::Max()}; //!< Estimated two-way round-trip propagation delay of the path, estimated
                      //!< from the windowed minimum recent round-trip delay sample.
    uint32_t m_sendQuantum{
        0}; //!< The maximum size of a data aggregate scheduled and transmitted together
    Time m_cycleStamp{Seconds(0)};       //!< Last time gain cycle updated
    uint32_t m_cycleIndex{0};            //!< Current index of gain cycle
    bool m_minRttExpired{false};         //!< A boolean recording whether the BBR.RTprop has expired
    Time m_minRttFilterLen{Seconds(5)};  //!< A constant specifying the length of the RTProp min
                                         //!< filter window, default 5 secs.
    Time m_minRttStamp{
        Seconds(0)}; //!< The wall clock time at which the current BBR.RTProp sample was obtained
    bool m_isInitialized{false}; //!< Set to true after first time initialization variables
    Ptr<UniformRandomVariable> m_uv{nullptr}; //!< Uniform Random Variable
    uint64_t m_delivered{0};   //!< The total amount of data in bytes delivered so far
    uint64_t m_deliveredCe{0}; //!< The total amount of ECE marked data in bytes delivered so far
    uint64_t m_lost{0};        //!< The total amount of lost data in bytes
    uint32_t m_appLimited{
        0}; //!< The index of the last transmitted packet marked as application-limited
    uint32_t m_extraAckedGain{1};         //!< Gain factor for adding extra ack to cwnd
    uint32_t m_extraAcked[2]{0, 0};       //!< Maximum excess data acked in epoch
    uint32_t m_extraAckedWinRtt{0};       //!< Age of extra acked in rtt
    uint32_t m_extraAckedWinRttLength{5}; //!< Window length of extra acked window
    uint32_t m_ackEpochAckedResetThresh{
        1 << 17}; //!< Max allowed val for m_ackEpochAcked, after which sampling epoch is reset
    uint32_t m_extraAckedIdx{0};     //!< Current index in extra acked array
    Time m_ackEpochTime{Seconds(0)}; //!< Starting of ACK sampling epoch time
    uint32_t m_ackEpochAcked{0};     //!< Bytes ACked in sampling epoch
    bool m_hasSeenRtt{false};        //!< Have we seen RTT sample yet?
    double m_pacingMargin{0.01}; //!< BBR intentionally reduces the pacing rate by 1% to drain any
                                 //!< standing queues. See `bbr_rate_bytes_per_sec` in Linux.
    double m_lossThresh{0.02};   //!< Estimate bw probing has gone too far if loss rate exceeds this level.
    uint32_t m_fullLossCount{8}; //!< Exit STARTUP if number of loss marking events in a Recovery round
                                 //!< is >= m_fullLossCount, and loss rate is higher than m_lossThresh.
    bool m_lossInCycle{false};        //!< Did packet loss happen in this cycle?
    bool m_lossInRound{false};        //!< Was there loss marked packet in the round trip?
    bool m_lossRoundStart{false};     //!< m_lossRoundDelivered round trip?
    uint32_t m_lossRoundDelivered{0}; //!< 
    uint32_t m_lossEventsInRound{0};  //!< losses in STARTUP round
    double m_ecnAlphaGain{0.0625};     //!< Gain factor for ECN mark ratio samples
    double m_ecnFactor{0.333};         //!< On ECN, cut inflight_lo to (1 - m_ecnFactor * m_ecnAlpha)
    double m_ecnThresh{0.5};           //!< Estimate bw probing has gone too far if CE ratio exceeds this threshold
    Time m_ecnMaxRtt{MilliSeconds(5)}; //!< Max RTT at which to use sender-side ECN logic
    uint32_t m_fullEcnCount{2};        //!< Exit STARTUP if number of ECN marked round trips with ECN mark rate 
                                       //!< above m_ecnThresh meets this count
    uint32_t m_startupEcnRounds{0}; //!< consecutive hi ECN STARTUP rounds
    bool m_ecnInCycle{false};       //!< ECN in this cycle?
    bool m_ecnEligible{false};      //!< sender can use ECN (RTT, handshake)?
    double m_ecnAlpha{1.0};         //!< EWMA delivered_ce/delivered
    bool m_ecnInRound{false};       //!< ECN marked in this round trip?
    uint32_t m_alphaLastDelivered{0};   //!< m_delivered at alpha update
    uint32_t m_alphaLastDeliveredCe{0}; //!< m_deliveredCe at alpha update
    double m_beta{0.3}; //!< On losses, scale down inflight and pacing rate by beta
    uint32_t m_inflightHi{static_cast<uint32_t>(-1)}; //!< Upper bound of inflight data range
    uint32_t m_inflightLo{static_cast<uint32_t>(-1)}; //!< Lower bound of inflight data range
    uint32_t m_inflightLatest{0};                     //!< Max delivered data in last round trip
    uint32_t m_bwLo{0};
    uint32_t m_bwHi[2]{0, 0};
    uint32_t m_bwLatest{0};
};

} // namespace ns3
#endif // TCPBBRV2_H
