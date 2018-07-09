//////////////////////////////////////////////////////////////////////////////////////
//
// (C) Daniel Strano and the Qrack contributors 2017, 2018. All rights reserved.
//
// This is a multithreaded, universal quantum register simulation, allowing
// (nonphysical) register cloning and direct measurement of probability and
// phase, to leverage what advantages classical emulation of qubits can have.
//
// Licensed under the GNU General Public License V3.
// See LICENSE.md in the project root or https://www.gnu.org/licenses/gpl-3.0.en.html
// for details.

#pragma once

#include <algorithm>
#include <memory>

#include "qinterface.hpp"

#include "common/parallel_for.hpp"

#define ALIGN_SIZE 64

namespace Qrack {

class QEngineCPU;
typedef std::shared_ptr<QEngineCPU> QEngineCPUPtr;

template <class BidirectionalIterator>
void reverse(BidirectionalIterator first, BidirectionalIterator last, bitCapInt stride);
template <class BidirectionalIterator>
void rotate(BidirectionalIterator first, BidirectionalIterator middle, BidirectionalIterator last, bitCapInt stride);

/**
 * General purpose QEngineCPU implementation
 */
class QEngineCPU : public QInterface, public ParallelFor {
protected:
    bool doNormalize;
    real1 runningNorm;
    complex* stateVec;

public:
    QEngineCPU(bitLenInt qBitCount, bitCapInt initState, std::shared_ptr<std::default_random_engine> rgp = nullptr,
        complex phaseFac = complex(-999.0, -999.0), bool partialInit = false);
    QEngineCPU(QEngineCPUPtr toCopy);
    ~QEngineCPU() { delete[] stateVec; }

    virtual void EnableNormalize(bool doN) { doNormalize = doN; }

    virtual void SetQuantumState(complex* inputState);
    virtual void SetPermutation(bitCapInt perm) { SetReg(0, qubitCount, perm); }

    virtual bitLenInt Cohere(QInterfacePtr toCopy) { return Cohere(std::dynamic_pointer_cast<QEngineCPU>(toCopy)); }
    std::map<QInterfacePtr, bitLenInt> Cohere(std::vector<QInterfacePtr> toCopy);

    virtual void Decohere(bitLenInt start, bitLenInt length, QInterfacePtr dest);

    virtual bitLenInt Cohere(QEngineCPUPtr toCopy);
    virtual void Dispose(bitLenInt start, bitLenInt length);

    /**
     * \defgroup BasicGates Basic quantum gate primitives
     *@{
     */

    virtual void ApplySingleBit(const complex* mtrx, bool doCalcNorm, bitLenInt qubitIndex);
    virtual void CCNOT(bitLenInt control1, bitLenInt control2, bitLenInt target);
    virtual void AntiCCNOT(bitLenInt control1, bitLenInt control2, bitLenInt target);
    virtual void CNOT(bitLenInt control, bitLenInt target);
    virtual void AntiCNOT(bitLenInt control, bitLenInt target);
    virtual void H(bitLenInt qubitIndex);
    virtual bool M(bitLenInt qubitIndex);
    virtual void X(bitLenInt qubitIndex);
    virtual void Y(bitLenInt qubitIndex);
    virtual void Z(bitLenInt qubitIndex);
    virtual void CY(bitLenInt control, bitLenInt target);
    virtual void CZ(bitLenInt control, bitLenInt target);

    /** @} */

    /**
     * \defgroup RotGates Rotational gates:
     *
     * NOTE: Dyadic operation angle sign is reversed from radian rotation
     * operators and lacks a division by a factor of two.
     *
     * @{
     */

    virtual void RT(real1 radians, bitLenInt qubitIndex);
    virtual void RX(real1 radians, bitLenInt qubitIndex);
    virtual void RY(real1 radians, bitLenInt qubitIndex);
    virtual void RZ(real1 radians, bitLenInt qubitIndex);
    virtual void Exp(real1 radians, bitLenInt qubitIndex);
    virtual void ExpX(real1 radians, bitLenInt qubitIndex);
    virtual void ExpY(real1 radians, bitLenInt qubitIndex);
    virtual void ExpZ(real1 radians, bitLenInt qubitIndex);
    virtual void CRX(real1 radians, bitLenInt control, bitLenInt target);
    virtual void CRY(real1 radians, bitLenInt control, bitLenInt target);
    virtual void CRZ(real1 radians, bitLenInt control, bitLenInt target);
    virtual void CRT(real1 radians, bitLenInt control, bitLenInt target);

    /** @} */

    /**
     * \defgroup RegGates Register-spanning gates
     *
     * Convienence and optimized functions implementing gates are applied from
     * the bit 'start' for 'length' bits for the register.
     *
     * @{
     */

    using QInterface::H;
    virtual void X(bitLenInt start, bitLenInt length);
    virtual void CNOT(bitLenInt control, bitLenInt target, bitLenInt length);
    virtual void AntiCNOT(bitLenInt control, bitLenInt target, bitLenInt length);
    virtual void CCNOT(bitLenInt control1, bitLenInt control2, bitLenInt target, bitLenInt length);
    virtual void AntiCCNOT(bitLenInt control1, bitLenInt control2, bitLenInt target, bitLenInt length);
    virtual void AND(bitLenInt inputBit1, bitLenInt inputBit2, bitLenInt outputBit, bitLenInt length);
    virtual void OR(bitLenInt inputBit1, bitLenInt inputBit2, bitLenInt outputBit, bitLenInt length);
    virtual void XOR(bitLenInt inputBit1, bitLenInt inputBit2, bitLenInt outputBit, bitLenInt length);

    /** @} */

    /**
     * \defgroup ArithGate Arithmetic and other opcode-like gate implemenations.
     *
     * @{
     */

    virtual void ROL(bitLenInt shift, bitLenInt start, bitLenInt length);
    virtual void ROR(bitLenInt shift, bitLenInt start, bitLenInt length);
    virtual void INC(bitCapInt toAdd, bitLenInt start, bitLenInt length);
    virtual void INCC(bitCapInt toAdd, bitLenInt start, bitLenInt length, bitLenInt carryIndex);
    virtual void INCS(bitCapInt toAdd, bitLenInt start, bitLenInt length, bitLenInt overflowIndex);
    virtual void INCSC(
        bitCapInt toAdd, bitLenInt start, bitLenInt length, bitLenInt overflowIndex, bitLenInt carryIndex);
    virtual void INCSC(bitCapInt toAdd, bitLenInt start, bitLenInt length, bitLenInt carryIndex);
    virtual void INCBCD(bitCapInt toAdd, bitLenInt start, bitLenInt length);
    virtual void INCBCDC(bitCapInt toAdd, bitLenInt start, bitLenInt length, bitLenInt carryIndex);
    virtual void DEC(bitCapInt toSub, bitLenInt start, bitLenInt length);
    virtual void DECC(bitCapInt toSub, bitLenInt start, bitLenInt length, bitLenInt carryIndex);
    virtual void DECS(bitCapInt toAdd, bitLenInt start, bitLenInt length, bitLenInt overflowIndex);
    virtual void DECSC(
        bitCapInt toAdd, bitLenInt start, bitLenInt length, bitLenInt overflowIndex, bitLenInt carryIndex);
    virtual void DECSC(bitCapInt toAdd, bitLenInt start, bitLenInt length, bitLenInt carryIndex);
    virtual void DECBCD(bitCapInt toAdd, bitLenInt start, bitLenInt length);
    virtual void DECBCDC(bitCapInt toSub, bitLenInt start, bitLenInt length, bitLenInt carryIndex);

    /** @} */

    /**
     * \defgroup ExtraOps Extra operations and capabilities
     *
     * @{
     */

    virtual void ZeroPhaseFlip(bitLenInt start, bitLenInt length);
    virtual void CPhaseFlipIfLess(bitCapInt greaterPerm, bitLenInt start, bitLenInt length, bitLenInt flagIndex);
    virtual void PhaseFlip();
    virtual void SetReg(bitLenInt start, bitLenInt length, bitCapInt value);
    virtual bitCapInt MReg(bitLenInt start, bitLenInt length);
    virtual bitCapInt IndexedLDA(bitLenInt indexStart, bitLenInt indexLength, bitLenInt valueStart,
        bitLenInt valueLength, unsigned char* values);
    virtual bitCapInt IndexedADC(bitLenInt indexStart, bitLenInt indexLength, bitLenInt valueStart,
        bitLenInt valueLength, bitLenInt carryIndex, unsigned char* values);
    virtual bitCapInt IndexedSBC(bitLenInt indexStart, bitLenInt indexLength, bitLenInt valueStart,
        bitLenInt valueLength, bitLenInt carryIndex, unsigned char* values);
    virtual void Swap(bitLenInt qubitIndex1, bitLenInt qubitIndex2);
    virtual void Swap(bitLenInt start1, bitLenInt start2, bitLenInt length);

    /** @} */

    /**
     * \defgroup UtilityFunc Utility functions
     *
     * @{
     */

    virtual complex* GetState();
    virtual void CopyState(QInterfacePtr orig);
    virtual real1 Prob(bitLenInt qubitIndex);
    virtual real1 ProbAll(bitCapInt fullRegister);
    virtual void SetBit(bitLenInt qubitIndex1, bool value);
    virtual real1 GetNorm(bool update = true)
    {
        if (update) {
            UpdateRunningNorm();
        }
        return runningNorm;
    }
    virtual void SetNorm(real1 n) { runningNorm = n; }
    virtual void NormalizeState(real1 nrm = -999.0);
    virtual bool ForceM(bitLenInt qubitIndex, bool result, bool doForce = true, real1 nrmlzr = 1.0);

    /** @} */

protected:
    virtual void ResetStateVec(complex* nStateVec);
    void DecohereDispose(bitLenInt start, bitLenInt length, QEngineCPUPtr dest);
    virtual void Apply2x2(bitCapInt offset1, bitCapInt offset2, const complex* mtrx, const bitLenInt bitCount,
        const bitCapInt* qPowersSorted, bool doCalcNorm);
    virtual void ApplyControlled2x2(bitLenInt control, bitLenInt target, const complex* mtrx, bool doCalcNorm);
    virtual void ApplyAntiControlled2x2(bitLenInt control, bitLenInt target, const complex* mtrx, bool doCalcNorm);
    virtual void ApplyDoublyControlled2x2(
        bitLenInt control1, bitLenInt control2, bitLenInt target, const complex* mtrx, bool doCalcNorm);
    virtual void ApplyDoublyAntiControlled2x2(
        bitLenInt control1, bitLenInt control2, bitLenInt target, const complex* mtrx, bool doCalcNorm);
    virtual void UpdateRunningNorm();
    virtual complex* AllocStateVec(bitCapInt elemCount);
};
} // namespace Qrack