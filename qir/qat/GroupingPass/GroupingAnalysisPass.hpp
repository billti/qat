#pragma once
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "GroupingPass/GroupingPassConfiguration.hpp"
#include "Logging/ILogger.hpp"
#include "Profile/Profile.hpp"
#include "QatTypes/QatTypes.hpp"

#include "Llvm/Llvm.hpp"

#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace microsoft
{
namespace quantum
{

    struct GroupAnalysis
    {
        using BlockList = std::vector<llvm::BasicBlock*>;
        BlockList qc_cc_blocks{};
        BlockList qc_mc_cc_blocks{};
    };

    class GroupingAnalysisPass : public llvm::AnalysisInfoMixin<GroupingAnalysisPass>
    {
      public:
        using Result      = GroupAnalysis;
        using Instruction = llvm::Instruction;
        using Value       = llvm::Value;
        using ILoggerPtr  = std::shared_ptr<ILogger>;
        using BlockSet    = std::unordered_set<llvm::BasicBlock*>;

        static String const QIS_START;
        static String const READ_INSTR_START;

        // Construction and destruction configuration.
        //

        explicit GroupingAnalysisPass(GroupingPassConfiguration const& cfg)
          : config_{cfg}
        {
        }

        /// Copy construction is banned.
        GroupingAnalysisPass(GroupingAnalysisPass const&) = delete;

        /// We allow move semantics.
        GroupingAnalysisPass(GroupingAnalysisPass&&) = default;

        /// Default destruction.
        ~GroupingAnalysisPass() = default;

        Result run(llvm::Module& module, llvm::ModuleAnalysisManager& mam);

        void runBlockAnalysis(llvm::Module& module);

        /// Whether or not this pass is required to run.
        static bool isRequired();

      private:
        GroupingPassConfiguration config_{};

        // Block classification
        //
        BlockSet contains_quantum_circuit_{};
        BlockSet contains_quantum_measurement_{};

        BlockSet pure_quantum_instructions_{};
        BlockSet pure_quantum_measurement_{};

        ILoggerPtr logger_{nullptr};

        static llvm::AnalysisKey Key;
        friend struct llvm::AnalysisInfoMixin<GroupingAnalysisPass>;
    };

    class GroupingAnalysisPassPrinter : public llvm::PassInfoMixin<GroupingAnalysisPassPrinter>
    {
      public:
        llvm::PreservedAnalyses run(llvm::Module& module, llvm::ModuleAnalysisManager& mam);

        static bool isRequired()
        {
            return true;
        }
    };

} // namespace quantum
} // namespace microsoft