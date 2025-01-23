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

    /**
     * \brief BBRv2 has the following 4 phases of PROBE_BW mode, defining the pacing gain:
     */
    enum BbrProbeBwPhase_t
    {
        BBR_BW_PROBE_UP,
        BBR_BW_PROBE_DOWN,
        BBR_BW_PROBE_CRUISE,
        BBR_BW_PROBE_REFILL
    };

    /**
     * \brief How does the incoming ACK stream relate to our bandwidth probing?
     */
    enum class BbrAckPhase_t
    {
        BBR_ACKS_INIT,
        BBR_ACKS_REFILLING,
        BBR_ACKS_PROBE_STARTING,
        BBR_ACKS_PROBE_FEEDBACK,
        BBR_ACKS_PROBE_STOPPING
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
    // void AdvanceCyclePhase();

    /**
     * \brief Checks whether to advance pacing gain in BBR_PROBE_BW state,
     *  and if allowed calls AdvanceCyclePhase ()
     * \param tcb the socket state.
     * \param rs rate sample.
     */
    // void CheckCyclePhase(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs);

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
     * \brief Calculate bdp based on min RTT and the estimated bottleneck bandwidth.
     * \param tcb the socket state.
     * \param bw bandwidth.
     * \param gain cwnd gain.
     * \return reutrns calculated bdp.
     */
    uint32_t GetBDP(Ptr<TcpSocketState> tcb, uint64_t bw, double gain);

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
    // void HandleRestartFromIdle(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs);

    /**
     * \brief Estimates the target value for congestion window
     * \param tcb  the socket state.
     * \param gain cwnd gain.
     * \return returns congestion window based on max bandwidth and min RTT.
     */
    uint32_t InFlight(Ptr<TcpSocketState> tcb, double gain);

    /**
     * \brief Estimates the target value for congestion window
     * \param tcb the socket state.
     * \param bw bandwidth.
     * \param gain cwnd gain.
     * \return returns congestion window based on max bandwidth and min RTT.
     */
    uint32_t InFlight(Ptr<TcpSocketState> tcb, uint64_t bw, double gain);

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
    // bool IsNextCyclePhase(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs);

    /**
     * \brief Modulates congestion window in BBR_PROBE_RTT.
     * \param tcb the socket state.
     */
    void ModulateCwndForProbeRTT(Ptr<TcpSocketState> tcb);

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
    void UpdateCongestionSignals(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs);

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
    void ProbeInflightHiUpward(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs);

    /**
     * \brief Loss and/or ECN rate is too high while probing.
     * Adapt (once per bw probe) by cutting inflight_hi and then restarting cycle.
     */
    void HandleInflightTooHigh(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs);

    /**
     * If we're seeing bw and loss samples reflecting our bw probing, adapt
     * using the signals we see. If loss or ECN mark rate gets too high, then adapt
     * m_inflightHi downward. If we're able to push inflight higher without such
     * signals, push higher: adapt m_inflightHi upward.
     * \param tcb the socket state.
     * \param rs rate sample.
     * \return returns true if state transition is decided.
     */
    bool AdaptUpperBounds(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs);

    /**
     * Estimate a short-term lower bound on the capacity available now, based
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
     * \param tcb the socket state.
     */
    void AdaptLowerBounds(Ptr<TcpSocketState> tcb);

    /**
     * \brief Does loss/ECN rate for this sample say inflight is "too high"?
     * \param tcb the socket state.
     * \param rs rate sample.
     * \return returns true if loss/ECN rate say inflight is "too high".
     */
    bool IsInflightTooHigh(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs);

    /**
     * \brief Return the windowed max recent bandwidth sample.
     */
    uint64_t MaxBandwidth();

    /**
     * \brief Calculate the bandwidth based on how fast packets are delivered.
     * \param rs rate sample.
     * \return returns bandwidth sample in bps.
     */
    uint64_t CalculateBwSample(const TcpRateOps::TcpRateSample& rs);

    /**
     * \brief Return the estimated bandwidth of the path.
     * \return estimated bandwidth, in bps.
     */
    uint64_t GetBandwidth();

    /**
     * \brief Updates PROBE_BW state, if it is allowed.
     * \param tcb the socket state.
     * \param rs rate sample.
     */
    void UpdateCyclePhase(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs);

    /**
     * \brief Check if it's time to probe for bandwidth now, and if so, kick it off.
     * \return returns true if we started probing.
     */
    bool CheckTimeToProbeBw(Ptr<TcpSocketState> tcb);

    /**
     * \brief Is it time to transition from PROBE_DOWN to PROBE_CRUISE?
     * \param tcb the socket state.
     * \param rs rate sample.
     * \return returns true if it's time to transition to PROBE_CRUISE.
     */
    bool CheckTimeToCruise(Ptr<TcpSocketState> tcb, const TcpRateOps::TcpRateSample& rs);

    /**
     * Send at estimated bw to fill the pipe, but not queue. We need this phase
     * before PROBE_UP, because as soon as we send faster than the available bw
     * we will start building a queue, and if the buffer is shallow we can cause
     * loss. If we do not fill the pipe before we cause this loss, our bw_hi and
     * inflight_hi estimates will underestimate.
     */
    void StartBwProbeRefill();

    /**
     * \brief Now probe max deliverable data rate and volume.
     * \param tcb the socket state.
     */
    void StartBwProbeUp(Ptr<TcpSocketState> tcb);

    /**
     * Start a new PROBE_BW probing cycle of some wall clock length. Pick a wall
     * clock time at which to probe beyond an inflight that we think to be
     * safe. This will knowingly risk packet loss, so we want to do this rarely, to
     * keep packet loss rates low. Also start a round-trip counter, to probe faster
     * if we estimate a Reno flow at our BDP would probe faster.
     */
    void StartBwProbeDown();

    /**
     * Cruise: maintain what we estimate to be a neutral, conservative
     * operating point, without attempting to probe up for bandwidth or down for
     * RTT, and only reducing inflight in response to loss/ECN signals.
     */
    void StartBwProbeCruise();

    /**
     * \brief Has the given amount of time elapsed since we marked the phase start?
     * \param interval the given amount of time.
     * \return returns true if the given amount of time elapsed.
     */
    bool HasElapsedInPhase(Time interval);

    /**
     * \brief How much do we want in flight? Our BDP, unless congestion cut cwnd.
     * \param tcb the socket state.
     * \return returns target value of data in flight.
     */
    uint32_t TargetInflight(Ptr<TcpSocketState> tcb);

    /**
     * \brief Each round trip of BBR_BW_PROBE_UP, double volume of probing data.
     * \param tcb the socket state.
     */
    void RaiseInflightHiSlope(Ptr<TcpSocketState> tcb);

    /**
     * \brief How long do we want to wait before probing for bandwidth (and risking
     * loss)? We randomize the wait, for better mixing and fairness convergence.
     */
    void PickProbeWait();

    /**
     * \return returns true if we're in state of probing for bandwidth.
     */
    bool IsProbingBandwidth();

    /**
     * If loss/ECN rates during probing indicated we may have overfilled a
     * buffer, return an operating point that tries to leave unutilized headroom in
     * the path for other flows, for fairness convergence and lower RTTs and loss.
     * \param tcb the socket state.
     * \param return returns data in flight, leaving headroom.
     */
    uint32_t InflightWithHeadroom(Ptr<TcpSocketState> tcb);

    /**
     * Bound cwnd to a sensible level, based on our current probing state
     * machine phase and model of a good inflight level (inflight_lo, inflight_hi).
     * \param tcb the socket state.
     */
    void BoundCwndForInflightModel(Ptr<TcpSocketState> tcb);

    /**
     * \brief Update m_pacingGain and m_cwndGain based on current mode.
     */
    void UpdateGains();

  private:
    BbrMode_t m_state{BbrMode_t::BBR_STARTUP}; //!< Current state of BBR state machine
    MaxBandwidthFilter_t m_maxBwFilter;        //!< Maximum bandwidth filter
    uint32_t m_bandwidthWindowLength{0}; //!< A constant specifying the length of the BBR.BtlBw max
                                         //!< filter window, default 10 packet-timed round trips.
    TracedValue<double> m_pacingGain{1}; //!< The dynamic pacing gain factor
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
    uint32_t m_priorCwnd{0};              //!< The last-known good congestion window
    bool m_idleRestart{false};            //!< When restarting from idle, set it true
    uint32_t m_targetCWnd{0}; //!< Target value for congestion window, adapted to the estimated BDP
    DataRate m_fullBandwidth{0};      //!< Value of full bandwidth recorded
    uint32_t m_fullBandwidthCount{0}; //!< Count of full bandwidth recorded consistently
    uint32_t m_sendQuantum{
        0}; //!< The maximum size of a data aggregate scheduled and transmitted together
    Time m_cycleStamp{Seconds(0)};       //!< Last time gain cycle updated
    uint32_t m_cycleIndex{0};            //!< Current index of gain cycle
    TracedValue<Time> m_minRtt{
        Time::Max()}; //!< Estimated two-way round-trip propagation delay of the path, estimated
                      //!< from the windowed minimum recent round-trip delay sample.
    bool m_minRttExpired{false};          //!< A boolean recording whether the BBR.RTprop has expired
    Time m_minRttFilterLen{Seconds(10)};  //!< A constant specifying the length of the RTProp min
                                          //!< filter window, default 10 secs.
    Time m_minRttStamp{
        Seconds(0)}; //!< The wall clock time at which the current BBR.RTProp sample was obtained
    TracedValue<Time> m_probeRttMin{
        Time::Max()}; //!< Minimum RTT in m_probeRttMinFilterLen window.
    bool m_probeRttMinExpired{false};         //!< A boolean recording whether the m_probeRttMin has expired
    Time m_probeRttMinFilterLen{Seconds(5)};  //!< A constant specifying the length of the probeRtt min
                                              //!< filter window, default 5 secs.
    Time m_probeRttMinStamp{
        Seconds(0)}; //!< The wall clock time at which the current m_porbeRttMin sample was obtained
    bool m_isInitialized{false}; //!< Set to true after first time initialization variables
    Ptr<UniformRandomVariable> m_uv{nullptr}; //!< Uniform Random Variable
    uint64_t m_delivered{0};   //!< The total amount of data in bytes delivered so far
    uint64_t m_deliveredCe{0}; //!< The total amount of ECE marked data in bytes delivered so far
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
    uint32_t m_lossRoundDelivered{0}; //!< m_delivered at loss round end
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
    uint64_t m_bwLo{static_cast<uint64_t>(-1)}; //!< Lower bound on sending bandwidth
    uint64_t m_bwLatest{0};                     //!< max delivered bw in last round trip
    uint32_t m_bwProbeUpCount{static_cast<uint32_t>(-1)}; //!< Bytes delivered per m_inflightHi increment
    uint32_t m_bwProbeUpAcks{0};   //!< Bytes (S)ACKed since m_inflightHi increment
    uint32_t m_bwProbeUpRounds{0}; //!< Cwnd-limited rounds in PROBE_UP
    bool m_stoppedRiskyProbe{false}; //!< Last PROBE_UP stopped due to risk?
    Time m_probeWait{Seconds(0)}; //!< PROBE_DOWN until next clock-driven probe
    bool m_prevProbeTooHigh{false}; //!< Did last PROBE_UP go too high?
    bool m_bwProbeSamples{false}; //!< Rate samples reflect bw probing?
    double m_inflightHeadroom{0.15}; //!< Fraction of unutilized headroom to try to leave in path upon high loss
    BbrAckPhase_t m_ackPhase{BbrAckPhase_t::BBR_ACKS_INIT}; //!< Relation of incoming ACK stream to the bandwidth probing
    Time m_bwProbeTimeBase{Seconds(2)};   //!< Use BBR-native probe time scale starting at this many seconds
    double m_bwProbeRandSeconds{1.0}; //!< Use BBR-native probes spread over this many seconds
    bool m_ecnEnabled{false}; //!< If false, disable support of ECN functionality
};

} // namespace ns3
#endif // TCPBBRV2_H
